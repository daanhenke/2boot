#!/usr/bin/env bash
set -eu

BUILD_DIR="build/uefi"

# --- Config (x86_64 / OVMF) ---
OVMF_BASE_URL="https://retrage.github.io/edk2-nightly/bin"
OVMF_CODE_NAME="RELEASEX64_OVMF_CODE.fd"
OVMF_VARS_NAME="RELEASEX64_OVMF_VARS.fd"

QEMU_BIN="${QEMU_BIN:-qemu-system-x86_64}"

# --- Paths ---
ROOT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
BUILD_DIR_ABS="$ROOT_DIR/$BUILD_DIR"
OVMF_DIR="$BUILD_DIR_ABS/ovmf"
ESP_DIR="dist"

OVMF_CODE_FD="$OVMF_DIR/$OVMF_CODE_NAME"
OVMF_VARS_FD="$OVMF_DIR/$OVMF_VARS_NAME"
OVMF_VARS_WORK_FD="$OVMF_DIR/OVMF_VARS.work.fd"

# --- Helpers ---
download() {
  url="$1"
  out="$2"

  if command -v curl >/dev/null 2>&1; then
    curl -fsSL "$url" -o "$out"
  elif command -v wget >/dev/null 2>&1; then
    wget -qO "$out" "$url"
  else
    echo "error: need curl or wget"
    exit 3
  fi
}

need_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "error: missing required command: $1"
    exit 4
  fi
}

# --- Checks ---
need_cmd "$QEMU_BIN"

# --- Fetch OVMF if needed ---
mkdir -p "$OVMF_DIR"

if [ ! -f "$OVMF_CODE_FD" ]; then
  echo "Downloading OVMF CODE: $OVMF_CODE_NAME"
  download "$OVMF_BASE_URL/$OVMF_CODE_NAME" "$OVMF_CODE_FD"
fi

if [ ! -f "$OVMF_VARS_FD" ]; then
  echo "Downloading OVMF VARS: $OVMF_VARS_NAME"
  download "$OVMF_BASE_URL/$OVMF_VARS_NAME" "$OVMF_VARS_FD"
fi

# Use a writable copy for VARS (UEFI NVRAM changes will be persisted here)
if [ ! -f "$OVMF_VARS_WORK_FD" ]; then
  cp "$OVMF_VARS_FD" "$OVMF_VARS_WORK_FD"
fi

echo "Launching QEMU..."

exec "$QEMU_BIN" \
  -machine q35 \
  -m 512M \
  -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE_FD" \
  -drive if=pflash,format=raw,file="$OVMF_VARS_WORK_FD" \
  -drive format=raw,file="fat:rw:$ESP_DIR" \
  -net none \
  -serial stdio
