@file:Suppress("MemberVisibilityCanBePrivate", "unused")

package io.github.vagran.adk.gradle

import org.gradle.api.Project
import java.io.File


open class AdkExtension(project: Project) {

    var cxx: String by AdkProperty(project, String::class.java,
                                   conventionValue = System.getenv("CXX"))

    var buildType: String by AdkProperty(project, String::class.java, BuildType.RELEASE.value,
        validator = {
            v ->
            if (v !in BuildType.values().map { it.value }) {
                throw Error("Unsupported build type: $v")
            }
        })

    var binType: String by AdkProperty(project, String::class.java, BinType.APP.value,
        validator = {
            v ->
            if (v !in BinType.values().map { it.value }) {
                throw Error("Unsupported build type: $v")
            }
        })

    var binName: String by AdkProperty(project, String::class.java,
                                       conventionValueProvider =  { project.name })

    internal val defineProp = AdkStringListProperty(project, emptyList())
    var define: List<String> by defineProp

    fun define(vararg items: String)
    {
        defineProp.Append(items)
    }

    internal val cflagsProp = AdkStringListProperty(project, emptyList())
    var cflags: List<String> by cflagsProp

    fun cflags(vararg items: String)
    {
        cflagsProp.Append(items)
    }

    internal val libsProp = AdkStringListProperty(project, emptyList())
    var libs: List<String> by libsProp

    fun libs(vararg items: String)
    {
        libsProp.Append(items)
    }

    var cppModuleIfaceExt: List<String> by AdkStringListProperty(project, listOf("cppm"))

    var cppImplExt: List<String> by AdkStringListProperty(project, listOf("cpp"))

    internal val includeProp = AdkFileListProperty(project, conventionValue = emptyList())
    var include: List<File> by includeProp

    fun include(vararg items: Any)
    {
        includeProp.Append(items)
    }

    internal val libDirProp = AdkFileListProperty(project, conventionValue =  emptyList())
    var libDir: List<File> by libDirProp

    fun libDir(vararg items: Any)
    {
        libDirProp.Append(items)
    }

    internal val modulesProp = AdkFileListProperty(project, conventionValue = emptyList())
    var modules: List<File> by modulesProp

    fun modules(vararg items: Any)
    {
        modulesProp.Append(items)
    }
}