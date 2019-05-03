# locate directories
SCRIPT_DIR=$(cd "$( dirname "$0" )" >/dev/null 2>&1 && pwd)
SOURCE_DIR=$(cd "$SCRIPT_DIR"/.. >/dev/null 2>&1 && pwd)
BUILD_DIR=${SOURCE_DIR}/build
SCRIPT_NAME=$(basename "$0")
PACKAGE_NAME=${SCRIPT_NAME%%-*}
PACKAGE_PREFIX=${SCRIPT_DIR}/${PACKAGE_NAME}

# recursion?
if [ "$1" == "-r" ]; then
  shift
  "${PACKAGE_PREFIX}-required" ${SCRIPT_NAME#*-} "$@"
fi
