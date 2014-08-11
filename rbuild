#!/bin/sh -e

HOST="${1:-$HOST}"

if [ -z "$HOST" ]; then
    echo "usage: $(basename $0) <host>" >&2
    exit 1
fi

rsync -v -rlp --exclude=.git ./ "$HOST:asterisk-lab"
ssh "$HOST" sh -e <<EOF
cd asterisk-lab
make
service asterisk stop
make install
service asterisk start
EOF
