#!/usr/bin/env bash
set -euo pipefail

REPOSITORY="${LBUG_GITHUB_REPOSITORY:-LadybugDB/ladybug}"
RUN_ID="${LBUG_JAVA_NATIVE_RUN_ID:-}"
TARGET_DIR="${LBUG_JAVA_NATIVE_TARGET_DIR:-src/main/resources}"

OS="$(uname -s)"
ARCH="$(uname -m)"

case "$OS:$ARCH" in
  Linux:x86_64)
    ARTIFACT_NAME="java-lib-linux-x86_64"
    ;;
  Linux:aarch64|Linux:arm64)
    ARTIFACT_NAME="java-lib-linux-aarch64"
    ;;
  Darwin:arm64)
    ARTIFACT_NAME="java-lib-osx-arm64"
    ;;
  MINGW*:x86_64|MSYS*:x86_64|CYGWIN*:x86_64)
    ARTIFACT_NAME="java-lib-win-x86_64"
    ;;
  *)
    echo "Unsupported platform for Java native libraries: $OS $ARCH" >&2
    exit 1
    ;;
esac

if [ -z "$RUN_ID" ]; then
  if ! command -v gh >/dev/null 2>&1; then
    echo "gh CLI is required when LBUG_JAVA_NATIVE_RUN_ID is not set" >&2
    exit 1
  fi
  while IFS= read -r candidate_run_id; do
    if gh api "/repos/${REPOSITORY}/actions/runs/${candidate_run_id}/artifacts" \
      --jq '.artifacts[].name' | grep -qx "$ARTIFACT_NAME"; then
      RUN_ID="$candidate_run_id"
      break
    fi
  done < <(gh run list --repo "$REPOSITORY" --workflow "Build and Deploy" --status success \
    --limit 20 --json databaseId --jq '.[].databaseId')
fi

if [ -z "$RUN_ID" ]; then
  echo "Unable to resolve a workflow run with artifact $ARTIFACT_NAME" >&2
  exit 1
fi

TMPDIR="$(mktemp -d)"
trap 'rm -rf "$TMPDIR"' EXIT

mkdir -p "$TARGET_DIR"
gh run download "$RUN_ID" --repo "$REPOSITORY" --name "$ARTIFACT_NAME" --dir "$TMPDIR"
find "$TMPDIR" -type f -name 'liblbug_java_native*' -exec cp {} "$TARGET_DIR/" \;

echo "Installed $ARTIFACT_NAME from $REPOSITORY run $RUN_ID into $TARGET_DIR"
