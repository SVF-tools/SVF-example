# -*- coding: utf-8 -*-
import sys, os, subprocess
from PyQt5.QtWidgets import (QWidget, QLabel, QApplication)
from main import *



class MyUI(QWidget, Ui_visual):
    def __init__(self, parent=None):
        super(MyUI, self).__init__(parent)
        self.setupUi(self)
        self.check.clicked.connect(self.doCheck)
        self.trans.clicked.connect(self.doTrans)
        self.git.clicked.connect(self.doGit)
        self.close.clicked.connect(self.doClose)
        self.clear.clicked.connect(self.doClear)

    def doCheck(self):
        # tools install
        statement = 'echo " " |sudo -S apt-get install zlib1g-dev unzip cmake gcc g++ nodejs libtinfo5 python3 npm'
        result = self.subprocess_popen(statement)
        if result == None:
            self.textEdit.setText('configure failed. command: "apt-get install zlib1g-dev unzip cmake gcc g++ nodejs libtinfo5 python3 npm"')
            return
        else:
            for res in result:
                self.textEdit.append(res)

        # npm install
        statement = 'npm i --silent svf-lib --prefix ${HOME}'
        self.textEdit.setText('npm installing...')
        result = self.subprocess_popen(statement)
        if result == None:
            self.textEdit.setText('configure failed. command: "npm i --silent svf-lib --prefix ${HOME}"')
            return
        else:
            for res in result:
                self.textEdit.append(res)

        # cmake make
        statement = 'cd SVF-Python-example && cmake . && make && clang -S -c -g -fno-discard-value-names -emit-llvm example.c -o example.ll'
        self.textEdit.setText('cmake make...')
        result = self.subprocess_popen(statement)
        if result == None:
            self.textEdit.setText('configure failed. command: "cmake . && make"')
            return
        else:
            for res in result:
                self.textEdit.append(res)


    def doTrans(self):
        tt = os.popen('cd /home/tp/PycharmProjects/llvm/svf-example-py-master1206 && python3 ./src/svf-ex.py ./example.ll')
        self.textEdit.setText(tt.read())

    def doGit(self):
        path = 'SVF-Python-example'
        if os.path.exists(path):
            os.remove(path)
        self.textEdit.setText('cloning...')
        result = self.subprocess_popen('git clone https://github.com/SVF-tools/SVF-Python-example')
        if result == None:
            self.textEdit.setText('git clone failed.')
        else:
            for res in result:
                self.textEdit.append(res)

    def doClose(self):
        app = QApplication.instance()
        app.quit()

    def doClear(self):
        self.textEdit.clear()

    def os_system(self, statement):
        result = os.system(statement)
        if result != 0:
            return None
        else:
            return result

    def subprocess_popen(self, statement):
        p = subprocess.Popen(statement, shell=True, stdout=subprocess.PIPE)
        while p.poll() is None:
            if p.wait() != 0:
                print("命令执行失败")
                return None
            else:
                re = p.stdout.readlines()
                result = []
                for i in range(len(re)):
                    res = re[i].decode('utf-8').strip('\r\n')
                    result.append(res)
                return result



if __name__ == '__main__':
    app = QApplication(sys.argv)
    myUI = MyUI()
    myUI.show()
    sys.exit(app.exec_())