# 聊天室后端 · 实现方案

## 1. 需求确认

| 维度 | 决策 |
|------|------|
| 语言 / 标准 | C++23，standalone **Asio**（`#include <asio.hpp>`，不链接 Boost） |
| 传输协议 | TCP，自定义文本协议，**JSON 行**（每行一个 JSON 对象） |
| 聊天模型 | **多房间 + 私聊** |
| 身份认证 | 暂不强制；预留 **HTTP token 认证接口**（客户端传 token → 后端调认证服务校验），当前用 stub 实现 |
| 数据存储 | 本地文件实现；用**接口抽象**，后续可替换为 Redis / MongoDB |
| 参考客户端 | 需要，命令行参考客户端 |
| 功能范围 | **完整功能**（见 §6 todo list） |

---

## 2. 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                      ChatServer (acceptor)                  │
│  io_context + 线程池，acceptor 监听端口，接受连接           │
│                                                             │
│   ┌──────────┐   ┌──────────┐   ┌─────────────────────────┐ │
│   │ Session  │──▶│ RoomHub  │   │        MessageStore     │ │
│   │(每连接)  │   │(房间注册)│   │   (存储接口抽象)        │ │
│   └──────────┘   └──────────┘   │   File / Redis / Mongo  │ │
│         │            │          └─────────────────────────┘ │
│         ▼            ▼                                      │
│   Authenticator   Message(JSON) ── 路由：广播 / 私聊 / 命令 │
│   (token 认证接口)                                          │
└─────────────────────────────────────────────────────────────┘
```

### 目录结构（规划）

沿用现有约定：头文件放 `include/`（以 `<子目录/头文件.h>` 命名空间路径包含），
实现文件统一用 `.cc` 放 `src/`（匹配 `file(GLOB_RECURSE SOURCES "src/*.cc")`）。

```
include/                        # 对外头文件（include path 根目录）
  server/
    chat_server.h               # acceptor + 会话生命周期管理
    session.h                   # 单连接读写、心跳、消息分帧（按行）
    room_hub.h                  # 房间注册/创建/删除、成员列表
  protocol/
    message.h                   # JSON 消息解析与序列化
    message_type.h              # 消息类型枚举 / 常量
  auth/
    authenticator.h             # 认证接口
    noop_authenticator.h        # stub：直接放行（预留 token 字段）
    http_authenticator.h        # TODO：调外部 HTTP 认证服务
  storage/
    message_store.h             # 存储接口
    file_store.h                # 本地文件实现（JSON 行落盘）
    redis_store.h               # TODO
    mongo_store.h               # TODO
  utils/
    datetime.h                  # 已存在
src/                            # 实现文件（*.cc）
  main.cc                       # 入口：解析参数、启动 server（原 main.cpp，见下注）
  server/
    chat_server.cc
    session.cc
    room_hub.cc
  protocol/
    message.cc
  auth/
    noop_authenticator.cc
    http_authenticator.cc
  storage/
    file_store.cc
client/
  chat_client.cpp               # 命令行参考客户端
tests/                          # GoogleTest 单元/集成测试（*.cc）
```

> 注：`src/main.cpp` 目前是 `.cpp`，且被 `cmakes/debug.cmake` / `release.cmake`
> 以 `"src/main.cpp"` 显式列出。若统一为 `.cc`，需同步把这两处路径改为
> `"src/main.cc"`；若保留 `.cpp`，则入口不参与 `src/*.cc` 的 glob，需继续显式列出。

---

## 3. 核心接口设计

### 3.1 认证接口 `Authenticator`

```cpp
namespace chat::auth {

struct AuthResult {
  bool ok = false;
  std::string user_id;
  std::string nickname;
  std::string error;          // 失败原因
};

// 认证服务抽象：当前 noop 直接放行，后续 http 实现调用远端服务
class Authenticator {
public:
  virtual ~Authenticator() = default;
  // token 为空时：返回匿名用户（ok=true，生成临时 user_id）
  virtual AuthResult verify(std::string_view token) = 0;
};

} // namespace chat::auth
```

- `NoopAuthenticator`：现在默认实现，任何 token 都返回 ok（或 token 为空走匿名）。
- `HttpAuthenticator`：TODO，预留。通过 HTTP 请求把 token 发往认证服务，返回用户信息。

### 3.2 存储接口 `MessageStore`

```cpp
namespace chat::storage {

struct Message {
  std::string id;             // 消息唯一 id
  std::string type;           // "room" / "private"
  std::string from;           // 发送者 user_id / 昵称
  std::string to;             // room_id 或接收者 user_id
  std::string content;        // 消息正文
  std::string ts;             // 时间戳字符串（ISO）
};

class MessageStore {
public:
  virtual ~MessageStore() = default;
  virtual void save(const Message& m) = 0;                 // 落盘
  virtual std::vector<Message> history(std::string_view room_id,
                                       std::size_t limit) = 0; // 房间历史
  virtual void save_offline(std::string_view user_id,
                            const Message& m) = 0;         // 离线私聊
  virtual std::vector<Message> drain_offline(std::string_view user_id) = 0;
};

} // namespace chat::storage
```

- `FileStore`：本地 JSON 行文件（如 `data/history/<room_id>.log`、`data/offline/<user_id>.log`），追加写。
- `RedisStore` / `MongoStore`：TODO，替换实现即可，接口不变。

---

## 4. 协议设计（JSON 行）

每条消息一行 JSON，`type` 字段区分。客户端 → 服务器为命令，服务器 → 客户端为推送/响应。

### 4.1 客户端 → 服务器（命令）

| type | 说明 | 字段 |
|------|------|------|
| `hello` | 连接握手（带 token） | `{token, nickname}` |
| `join` | 加入房间 | `{room_id}` |
| `leave` | 离开房间 | `{room_id}` |
| `room_list` | 列出所有房间 | — |
| `user_list` | 列出房间成员 | `{room_id}` |
| `room_msg` | 发房间广播 | `{room_id, content}` |
| `private_msg` | 发私聊 | `{to, content}` |
| `history` | 拉取房间历史 | `{room_id, limit}` |
| `room_create` | 创建房间 | `{room_id}` |
| `room_delete` | 删除房间（需权限） | `{room_id}` |
| `ping` | 心跳 | — |

### 4.2 服务器 → 客户端（推送/响应）

| type | 说明 |
|------|------|
| `ack` | 命令成功/失败响应 `{ok, error}` |
| `room_msg` | 房间广播推送给成员 |
| `private_msg` | 私聊推送给接收者 |
| `user_join` / `user_leave` | 房间成员进出通知 |
| `room_list` / `user_list` / `history` | 查询结果 |
| `offline_msg` | 上线后补投的离线私聊 |
| `error` | 协议/权限/限流错误 |
| `pong` | 心跳响应 |

### 4.3 示例

```
客户端 →: {"type":"hello","token":"xxx","nickname":"alice"}
服务器 ←: {"type":"ack","ok":true,"user_id":"u1"}
客户端 →: {"type":"room_create","room_id":"lobby"}
客户端 →: {"type":"join","room_id":"lobby"}
客户端 →: {"type":"room_msg","room_id":"lobby","content":"hi"}
服务器 ←(广播给 lobby 成员): {"type":"room_msg","from":"alice","room_id":"lobby","content":"hi","ts":"..."}
```

---

## 5. 并发模型

- 单 `io_context` + **线程池**（`asio::thread_pool` 或手动 `std::thread` 跑 `io_context`）。
- 每个 `Session` 绑定 **strand**，保证单连接内读写串行、无数据竞争。
- `RoomHub` 用 `std::shared_mutex` 或独立 strand 保护房间/成员表。
- `MessageStore` 落盘走**独立写队列/strand**，避免磁盘 IO 阻塞网络线程。

---

## 6. 待实现功能 Todo List

### 阶段一：基础框架与构建
- [ ] 整理 CMake 依赖：Asio / `nlohmann/json` / gtest 均走本地 include 路径（`ASIO_DIR` / `JSON_DIR` / `GTEST_SOURCE_DIR`），保持 C++23
- [ ] 建立目录结构（头文件 `include/{server,protocol,auth,storage}`，实现 `src/...`，`client/`）
- [ ] 定义 `protocol::Message` 与 JSON 行编解码（`message.h` / `message.cc`）

### 阶段二：存储层（接口 + 文件实现）
- [ ] 定义 `MessageStore` 接口（`save` / `history` / `save_offline` / `drain_offline`）
- [ ] 实现 `FileStore`：房间历史、离线私聊的 JSON 行落盘与读取
- [ ] 存储接口可注入（工厂/配置选择 File / 未来 Redis / Mongo）

### 阶段三：认证层（接口 + stub）
- [ ] 定义 `Authenticator` 接口与 `AuthResult`
- [ ] 实现 `NoopAuthenticator`（匿名 + token 放行）
- [ ] 预留 `HttpAuthenticator` 空实现与接入点（TODO 注释说明）

### 阶段四：会话与网络层
- [ ] `ChatServer`：acceptor 监听、接受连接、会话登记/清理
- [ ] `Session`：按行分帧读、strand 串行化、写队列、心跳超时踢出
- [ ] `hello` 握手：调用 `Authenticator.verify`，绑定 user_id / nickname

### 阶段五：房间与消息路由
- [ ] `RoomHub`：房间创建/删除/查询、成员加入/离开、成员列表
- [ ] `room_msg`：房间内广播（含时间戳）
- [ ] `private_msg`：在线直发；离线则写 `save_offline`，上线 `drain_offline` 补投
- [ ] `join`/`leave` 的 `user_join`/`user_leave` 通知

### 阶段六：查询与完整功能
- [ ] `room_list` / `user_list` 查询
- [ ] `history`：调 `MessageStore.history` 回放历史
- [ ] `room_create` / `room_delete`（含权限判断）
- [ ] 权限：普通用户 / 管理员（房主可删房等）
- [ ] 限流：单连接消息速率限制
- [ ] 心跳：`ping`/`pong` 与超时剔除

### 阶段七：参考客户端
- [ ] 命令行客户端：连接、`hello`（token）、加房、发消息、收推送
- [ ] 交互式命令（`/join` `/msg` `/rooms` 等）映射到 JSON 命令

### 阶段八：测试
- [ ] 协议编解码单元测试（`message` 往返）
- [ ] `FileStore` 单测（写入/历史/离线消息）
- [ ] `NoopAuthenticator` 单测
- [ ] `RoomHub` 单测（建/删房、成员、广播）
- [ ] 集成测试：启动 server + 多客户端连接、广播、私聊、离线补投
- [ ] `make run-tests` 全部通过

---

## 7. 非目标（本次不做）

- HTTP / WebSocket 接入（协议层预留，但本次仅 TCP）
- Redis / MongoDB 具体实现（仅留接口 + 工厂）
- 真正的 HTTP 认证服务对接（仅 stub + 接口）
