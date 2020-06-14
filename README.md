## 1. clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 2. Install SVF via npm
```
npm install svflib
```

You are able to check your installed npm package and its path via command `npm list`
```
$ npm list
/home/samsung
└── svflib@1.0.0
```
Then your_path_to_SVF is `/home/samsung/node_modules/SVF`.

## 3. cmake your project by pointing to SVF_DIR and LLVM_DIR
```
cmake -DSVF_DIR=your_path_to_SVF
make
```
