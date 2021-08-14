package io.github.vagran.adk.gradle

/** Provides info about compiler(s). Currently it is mostly fixed to clang compiler. */
class CompilerInfo(val adkConfig: AdkExtension) {
    val cxx: String

    val cppCompiledModuleExt: String = "pcm"
    val objFileExt: String = "o"

    init {
        if (adkConfig.cxx.isBlank()) {
            throw Error("C++ compiler binary must be specified")
        }
        cxx = adkConfig.cxx
    }
}