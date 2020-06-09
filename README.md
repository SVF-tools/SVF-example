## 1. clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 2. Download LLVM and SVF binaries

### 2.1 For Ubuntu user
```
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
wget https://github.com/charles32110/SVF_Lib/archive/V1.0.0.tar.gz
tar -xvf clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
tar -xvf SVF_Lib-1.0.0.tar.gz
```

### 2.2 For Mac user
```
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang+llvm-10.0.0-x86_64-apple-darwin.tar.xz
wget https://github.com/charles32110/SVF_Lib/archive/V1.0.0.tar.gz
tar -xvf clang+llvm-10.0.0-x86_64-apple-darwin.tar.xz
tar -xvf SVF_Lib-1.0.0.tar.gz
```

## 3. cmake your project by pointing to SVF_DIR and LLVM_DIR
```
cmake -DSVF_DIR=your_path_to/SVF_Lib-1.0.0 -DLLVM_DIR=your_path_to/llvm-10.0.0.obj
make
```
