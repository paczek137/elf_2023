SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd ${SCRIPT_DIR}
PROJECT_NAME=$( echo ${SCRIPT_DIR} | rev | cut -d'/' -f 1 | rev )
echo ${PROJECT_NAME}
g++ -std=c++20 -o ${PROJECT_NAME}.o main.cpp && ./${PROJECT_NAME}.o