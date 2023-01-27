FROM gcc:4.9

LABEL VERSION=1.0
LABEL DESCRIPCION="Programas C/C++"

COPY . /usr/src/app

WORKDIR /usr/src/app

RUN g++ base64.cpp -o base64

CMD ["./base64"]
# CMD [ "tail", "-f", "logs/logs.log" ]
