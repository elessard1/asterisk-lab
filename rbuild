#!/bin/sh

HOST="${HOST:-xivo}"

rsync -v -rtlp --exclude=.git ./ "$HOST":asterisk-lab
ssh "$HOST" sh -e <<EOF
cd asterisk-lab
make
/etc/init.d/asterisk stop
make install
/etc/init.d/asterisk start
EOF
