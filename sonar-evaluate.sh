#!/bin/sh
 
export SONAR_SCANNER_VERSION=4.7.0.2747
export SONAR_SCANNER_HOME=$HOME/.sonar/sonar-scanner-$SONAR_SCANNER_VERSION-linux

wget https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
unzip -x build-wrapper-linux-x86.zip
mv build-wrapper-linux-x86/* .


curl --create-dirs -sSLo $HOME/.sonar/sonar-scanner.zip https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-$SONAR_SCANNER_VERSION-linux.zip
unzip -o $HOME/.sonar/sonar-scanner.zip -d $HOME/.sonar/
export PATH=$SONAR_SCANNER_HOME/bin:$PATH
export SONAR_SCANNER_OPTS="-server"

curl --create-dirs -sSLo $HOME/.sonar/build-wrapper-linux-x86.zip https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
unzip -o $HOME/.sonar/build-wrapper-linux-x86.zip -d $HOME/.sonar/
export PATH=$HOME/.sonar/build-wrapper-linux-x86:$PATH

export SONAR_TOKEN=f9cb0c86243901a508b93c402ae9ebd421302e00

./build-wrapper-linux-x86-64 --out-dir bw-output g++ src/cksumcrc32.cpp -o cksumcrc32

sonar-scanner \
  -Dsonar.organization=jonnattan-org \
  -Dsonar.projectKey=jonnattangc_test-c \
  -Dsonar.sources=. \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=https://sonarcloud.io


  rm -rf build-wrapper-linux-x86/
  rm -rf bw-output/
  rm -rf *.so
  rm -f cksumcrc32

