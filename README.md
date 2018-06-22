## alfredos

## Compile
```shell
$ cmake -Hsrc -Bbuild
$ cmake --build build
```
This is a out-of-source build and output binary is under build/ directory.

## Run
```shell
$ ./build/alfredos
```

**Example Usage:**
```shell
$ send demo1 1 this is a test data
tid(7fcdd8ca8700) demo1 executes cmd_id 1 with cmd_data "this is a test data"
$ test 10
Send messages to running threads:
tid(7fcdd94a9700) demo2 executes cmd_id 0 with cmd_data "iteration 0"
tid(7fcdd8ca8700) demo1 executes cmd_id 0 with cmd_data "iteration 0"
tid(7fcdd8ca8700) demo1 executes cmd_id 1 with cmd_data "iteration 0"
tid(7fcdd8ca8700) demo1 executes cmd_id 0 with cmd_data "iteration 1"
...
```
