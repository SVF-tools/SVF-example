## 1. clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 2. Install SVF via npm
```
sudo npm install -g svflib
```

## 3. cmake your project by pointing to SVF_DIR and LLVM_DIR
```
cmake -DSVF_DIR=your_path_to_SVF
make
```
