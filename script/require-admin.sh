if [ `uname -o` == 'Msys' ]; then
  net session >&/dev/null
  if [[ $? != 0 ]]; then
    echo Please run this script as administrator.
    exit 1
  fi
else
  if [ `id -u` != 0 ]; then
    echo Please run this script as root.
    exit 1
  fi
fi
