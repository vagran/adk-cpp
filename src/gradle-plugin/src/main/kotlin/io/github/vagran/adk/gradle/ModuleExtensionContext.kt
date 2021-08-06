package io.github.vagran.adk.gradle

import org.gradle.api.Project
import java.io.File

class ModuleExtensionContext(project: Project, val baseDir: File) {

    var name: String by AdkProperty(project, String::class.java)

    val includeProp = AdkFileListProperty(project, baseDir = baseDir)
    var include: List<File> by includeProp

    val libDirProp = AdkFileListProperty(project, baseDir = baseDir)
    var libDir: List<File> by libDirProp

    val submodulesProp = AdkFileListProperty(project, baseDir = baseDir)
    var submodules: List<File> by submodulesProp

    val implProp = AdkFileListProperty(project, baseDir = baseDir)
    var impl: List<File> by implProp

    val moduleMapProp = AdkFileListProperty(project, baseDir = baseDir)
    var moduleMap: List<File> by moduleMapProp

    val dependsProp = AdkStringListProperty(project)
    var depends: List<String> by dependsProp

    val defineProp = AdkStringListProperty(project)
    var define: List<String> by defineProp

    val cflagsProp = AdkStringListProperty(project)
    var cflags: List<String> by cflagsProp

    val libsProp = AdkStringListProperty(project)
    var libs: List<String> by libsProp

    val childContexts = ArrayList<ModuleExtensionContext>()
}