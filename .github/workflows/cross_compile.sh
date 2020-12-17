#! /bin/bash

targets=${@}

for target in "${targets}"; do
    docker run -t --rm --cap-add SYS_PTRACE -v $(pwd):/src docker.pkg.github.com/mihaigalos/docker/avr-gcc  /bin/zsh -c \
        'cd /src && ./build.sh'
done
