FROM gcc:4.9

LABEL VERSION=1.0
LABEL DESCRIPCION="Programas C/C++"

COPY . /usr/src/app

WORKDIR /usr/src/app

RUN g++ src/cksumcrc32.cpp -o cksumcrc32

CMD ["./cksumcrc32"]
# CMD [ "tail", "-f", "logs/logs.log" ]
