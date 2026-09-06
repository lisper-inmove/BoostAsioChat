#!/bin/bash
# 自动从测试二进制中提取所有 gtest 用例名，生成 Makefile
# 用法: ./gen_test_makefile.sh [build_root]    默认 build_root = build/

set -euo pipefail

TEST_NAME=ChatServerTest # 与 main.cmake 中的命名一致
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# build 根目录；debug / release / test 各自使用独立的子目录
if [[ -n "${1:-}" ]]; then
  case "$1" in
    /*) BUILD_ROOT="$1" ;;
    *)  BUILD_ROOT="$SCRIPT_DIR/$1" ;;
  esac
else
  BUILD_ROOT="$SCRIPT_DIR/build"
fi

DEBUG_DIR="$BUILD_ROOT/debug"
RELEASE_DIR="$BUILD_ROOT/release"
TEST_DIR="$BUILD_ROOT/test"
TEST_BIN="$TEST_DIR/${TEST_NAME}"
MAKEFILE="$SCRIPT_DIR/Makefile"

# 转义 sed 替换串中的特殊字符（& \ 以及分隔符 |）
sed_escape() { printf '%s' "$1" | sed 's/[&\\|]/\\&/g'; }

#############################################################################

# ── 配置 + 编译（始终重新编译，确保测试列表最新）──────────────────────────

echo "[gen] 配置 Test 构建目录: $TEST_DIR"
cmake -S "$SCRIPT_DIR" -B "$TEST_DIR" -DCMAKE_BUILD_TYPE=Test || {
  echo "[gen] 配置失败"
  exit 1
}

echo "[gen] 正在编译..."
cmake --build "$TEST_DIR" --target all -- -j"$(nproc)" || {
  echo "[gen] 编译失败"
  exit 1
}

if [[ ! -x "$TEST_BIN" ]]; then
  echo "[gen] 找不到测试二进制: $TEST_BIN"
  echo "[gen] 请先构建 Test 类型: make test"
  exit 1
fi

echo "[gen] 从 $TEST_BIN 提取测试列表..."

# ── 解析 gtest 列表：套件名(非缩进) / 用例名(缩进)，支持多套件 ───────────

declare -a TEST_ENTRIES=()   # 每个元素为 "suite|test"
current_suite=""
while IFS= read -r line; do
  if [[ "$line" =~ ^[[:space:]] ]]; then
    name="${line#"${line%%[![:space:]]*}"}"   # 去前导空白
    if [[ -z "$name" || "$name" == \#* ]]; then
      continue                                 # 跳过空行 / 注释行
    fi
    name="${name%% *}"                          # 去参数化用例的内联注释
    if [[ -n "$current_suite" ]]; then
      TEST_ENTRIES+=("$current_suite|$name")
    fi
  else
    current_suite="${line%.}"                    # 套件名，去掉末尾 '.'
  fi
done < <("$TEST_BIN" --gtest_list_tests 2>/dev/null)

if [[ ${#TEST_ENTRIES[@]} -eq 0 ]]; then
  echo "[gen] 未找到任何测试用例"
  exit 1
fi

echo "[gen] 找到 ${#TEST_ENTRIES[@]} 个测试用例"

# 统计跨套件重名的用例，用于消除 Make 目标冲突
declare -A NAME_COUNT=()
for entry in "${TEST_ENTRIES[@]}"; do
  n="${entry#*|}"
  NAME_COUNT[$n]=$(( ${NAME_COUNT[$n]:-0} + 1 ))
done

# 把非 [A-Za-z0-9_] 替换为 _，保证 Make 目标名合法
sanitize() { local s="$1"; echo "${s//[^A-Za-z0-9_]/_}"; }

# ── 生成 Makefile 头 ───────────────────────────────────────────────────────

cat >"$MAKEFILE" <<'HEADER'
# ============================================================================
#  自动生成的测试 Makefile
#  运行 ./gen_test_makefile.sh 重新生成
# ============================================================================

TEST_NAME := __TEST_NAME__
SRC_DIR   := __SRC_DIR__
BUILD_ROOT := __BUILD_ROOT__
DEBUG_DIR  := $(BUILD_ROOT)/debug
RELEASE_DIR := $(BUILD_ROOT)/release
TEST_DIR   := $(BUILD_ROOT)/test
TEST_BIN   := $(TEST_DIR)/$(TEST_NAME)

# 检测终端是否支持颜色
ifeq ($(shell test -t 1 && echo yes),yes)
  GREEN  := \033[0;32m
  RED    := \033[0;31m
  YELLOW := \033[0;33m
  RESET  := \033[0m
else
  GREEN  :=
  RED    :=
  YELLOW :=
  RESET  :=
endif

# ── list 目标 ──────────────────────────────────────────────────────────────

.PHONY: list
list:
	@echo "$(YELLOW)=== 可用测试目标 ($(words $(TESTS)) 个) ===$(RESET)"
	@$(foreach t,$(TESTS),echo "  make $(t)";)
	@echo "  make run-tests"

# ── help ───────────────────────────────────────────────────────────────────

.PHONY: help
help:
	@echo "用法:"
	@echo "  make <测试名>     运行单个测试"
	@echo "  make list         列出所有测试"
	@echo "  make build        编译测试二进制"
	@echo "  make regen        重新生成此 Makefile"

# ── build ──────────────────────────────────────────────────────────────────

.PHONY: build
build:
	@echo "[build] 编译 $(TEST_BIN)..."
	@cmake --build $(TEST_DIR) --target $(TEST_NAME) -- -j$$(nproc)

# ── regen ──────────────────────────────────────────────────────────────────

.PHONY: regen
regen:
	@$(SRC_DIR)/gen_test_makefile.sh

# ── 单个测试模板 ───────────────────────────────────────────────────────────

define TEST_TEMPLATE
.PHONY: $(1)
$(1): build
	@printf "$(YELLOW)[$(1)]$(RESET) "
	@$(TEST_BIN) --gtest_filter="$(2)" \
		&& printf "$(GREEN)PASS$(RESET)\n" \
		|| { printf "$(RED)FAIL$(RESET)\n"; exit 1; }
endef

TESTS :=

HEADER

sed -i "s|__TEST_NAME__|$(sed_escape "$TEST_NAME")|" "$MAKEFILE"
sed -i "s|__SRC_DIR__|$(sed_escape "$SCRIPT_DIR")|" "$MAKEFILE"
sed -i "s|__BUILD_ROOT__|$(sed_escape "$BUILD_ROOT")|" "$MAKEFILE"

# ── 追加每个测试的规则（含正确的套件 filter）──────────────────────────────

for entry in "${TEST_ENTRIES[@]}"; do
  suite="${entry%%|*}"
  name="${entry#*|}"
  if [[ ${NAME_COUNT[$name]} -gt 1 ]]; then
    target="$(sanitize "$suite")__${name}"   # 跨套件重名：加套件前缀
  else
    target="$name"
  fi
  filter="${suite}.${name}"
  cat >>"$MAKEFILE" <<EOF
TESTS += $target
\$(eval \$(call TEST_TEMPLATE,$target,$filter))

EOF
done

# ── 额外构建目标（debug/release/test 各自独立构建目录）────────────────────

cat >>"$MAKEFILE" <<'EXTRA'
# ── 构建类型目标 ───────────────────────────────────────────────────────────

.PHONY: debug
debug:
	cmake -S $(SRC_DIR) -B $(DEBUG_DIR) -DCMAKE_BUILD_TYPE=Debug \
		&& cmake --build $(DEBUG_DIR) -j$$(nproc) \
		&& cp $(DEBUG_DIR)/compile_commands.json $(SRC_DIR)/

.PHONY: release
release:
	cmake -S $(SRC_DIR) -B $(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release \
		&& cmake --build $(RELEASE_DIR) -j$$(nproc)

.PHONY: test
test:
	cmake -S $(SRC_DIR) -B $(TEST_DIR) -DCMAKE_BUILD_TYPE=Test \
		&& cmake --build $(TEST_DIR) -j$$(nproc)

.PHONY: run-tests
run-tests:
	@echo "$(YELLOW)=== 运行全部测试 ===$(RESET)"
	@$(TEST_BIN)

.PHONY: generate
generate: test
	$(SRC_DIR)/gen_test_makefile.sh

EXTRA

cat >>"$MAKEFILE" <<'FOOTER'

# ============================================================================
FOOTER

echo "[gen] Makefile 已生成: $MAKEFILE"
echo "[gen] 可用命令: make list | make <test-name> | make run-tests"
