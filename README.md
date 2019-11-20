# 研究 SO 文件中的算法 (上)

目前主流 APP ，都会把加密算法放入 SO 文件中，以防止用户逆向。所以研究 APP 算法，最简单的办法就是新建一个 Android 项目，并引入该 SO 进行调试 ( 需要该 SO 文件没有验证机制 ) 。

本文分为 3 篇，简要介绍怎样研究一个 APP 的 SO 算法。第一篇主要介绍 Android NDK 的开发流程，让读者对于 SO 开发有一个直观理解。第二篇讲述怎样引入一个没有源文件的 SO，进行编译运行。第三篇则会介绍通过 IDA Pro 调试 SO 文件。

这是第一篇，分为两个部分，第一部分是建立一个基础的 Android 项目，第二部分是引入 C 文件并运行。

### 新建一个 Android 空项目

首先新建一个没有 NDK 的 Android 的项目，完全参考安卓 [官方教程](https://developer.android.com/training/basics/firstapp)

![](https://i.loli.net/2019/11/20/kgKLudpX69DycB2.png)

其次新建一个输入框，一个按键和一个文本输出框，Android 程序的布局如下图所示:

![](https://i.loli.net/2019/11/20/Ey1StPqugMJmAk7.png)

紧接着在 MainActivity 中添加 calMessage 函数并关联到 Button 上

```java
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void calMessage(View view) {
        TextView editText = findViewById(R.id.editText);
        String msg = editText.getText().toString();
        msg = "trans msg: " + msg;
        TextView showText = findViewById(R.id.textView);
        showText.setText(msg);
        editText.setText("");
    }
}
```

最后运行，结果如下:

![](https://i.loli.net/2019/11/20/GhkSvDcVKPxI1XJ.png)

### 引入 NDK

现在我们来改造一下这个 APP，使得按下按钮的时候，可以调用 C 语言将字符串反转。

Android 目前有两种引入方式，分别是通过 CMake 和 ndk-build。官方推荐使用 CMake，因此我们这里也使用 CMake 进行引入。

首先在项目中新建一个 CPP 目录 `app/src/main/cpp`，并在其中添加两个文件

第一个是用于 CMake 的 **CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.4.1)
add_library(translib SHARED translib.c)
target_link_libraries(translib)
```

第二个就是用于反转字符串的 **translib.c**

```c
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

char *trans(char *);

JNIEXPORT jstring JNICALL
Java_com_example_sodemo_MainActivity_transMsg(JNIEnv *env, jobject obj, jstring inputMsg) {
    char *inputStr = (char *) (*env)->GetStringUTFChars(env, inputMsg, 0);
    char *outputStr = trans(inputStr);
    (*env)->ReleaseStringUTFChars(env, inputMsg, inputStr);
    return (*env)->NewStringUTF(env, outputStr);
}

char *trans(char *msg) {
    size_t msg_len = strlen(msg);
    char *result = malloc(msg_len + 1); //+1 for the zero-terminator
    for (int i = 0; i < msg_len; i++) {
        result[i] = msg[msg_len - i - 1];
    }
    return result;
}
```

_注意:_ 其中的函数命名 **Java_com_example_sodemo_MainActivity_transMsg** 需要符合 JNI 的命名规范，也就是必须包含 Android 包名。以本项目为例，包名为 **com.example.sodemo**, 所以函数名称必须以 **Java_com_example_sodemo** 开头

然后在 Android Studio -> File 中选择 **Link C++ Project**，如下图所示:

![](https://i.loli.net/2019/11/20/QofBARO57NJVh3i.png)

然后选择 `app/src/main/cpp/CMakeLists.txt` 即可

![](https://i.loli.net/2019/11/20/WQ8byhucdFaOHYk.png)

添加成功后，可以在 **MainActivity.java** 中加入如下代码，以引入 `translib` 的 SO 库。

```java
public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("translib");
    }

    public native String transMsg(String msg);
}
```

最终代码如下所示：

```java
public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("translib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void calMessage(View view) {
        TextView editText = findViewById(R.id.editText);
        String msg = editText.getText().toString();
        msg = "trans msg: " + transMsg(msg);
        TextView showText = findViewById(R.id.textView);
        showText.setText(msg);
        editText.setText("");
    }

    public native String transMsg(String msg);
}
```

通过点击 CAL 按钮，可以看到程序调用了 `translib` 库，最终运行结果如下:

![](https://i.loli.net/2019/11/20/1OxULVtvN26g9Qm.png)

至此，我们就成功的在一个 Android 项目中，引入了包含源文件的 SO 库。下一篇中，我们将继续讲述怎样引入没有源文件的 SO 库。
