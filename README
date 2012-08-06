# ParaView LAS Plugin

## Build Instructions

To create a plugin, one must have their own build of ParaView3. Binaries downloaded from www.paraview.org do not include the necessary header files or import libraries (where applicable) for compiling plug-ins. More information on writing ParaView plugins can be found [here](http://www.paraview.org/Wiki/ParaView/Plugin_HowTo).

The build process to compile and run the LAS plugin for ParaView is defined by three steps: 1) ***Building ParaView***, 2) ***Building libLAS*** and 3) ***Building the LAS plugin***.

### Download and install Paraview

Download the ParaView 3.14.1 source [tarball](http://www.paraview.org/paraview/resources/software.php) and follow the [build instructions](http://paraview.org/Wiki/ParaView:Build_And_Install) to compile ParaView. First, unpack the tarball, i.e.,

```
$ tar xvfz ParaView-3.14.1-Source.tar.gz
$ cd ParaView-3.14.1-Source
```

Then, create a Build directory and call `ccmake` to enter a graphical user interface for changing cmake variables. Alternatively, you can also set the variables on the command line.

```
$ mkdir Build
$ cd Build
$ ccmake ..
```

**Note:** Make sure that `BUILD_SHARED_LIBS` is set to `ON` (default is `OFF`). Hitting `'c'` followed by `'g'` will configure and generate all files for the build process. Running

```
$ make
```

will build ParaView as well as all the submodules (such as VTK). Optionally, you can specify `-jN`, e.g., make `-j4` to speed up the compilation process by using more cores.

### Download and build libLAS

Download the libLAS 1.7.0 source [tarball[(http://download.osgeo.org/liblas/libLAS-1.7.0.tar.gz) and follow the [build instructions](http://www.liblas.org/compilation.html) to compile libLAS. First, unpack the tarball, i.e.,

```
$ tar xvfz libLAS-1.7.0-src.tar.gz
$ cd libLAS-1.7.0-src
```

For now, we assume the user has the following libLAS dependencies installed.

* Boost 1.38.0
* GDAL 1.7.0
* libgeotiff 1.3.0

Although these are the published requirements, we tested with Boost 1.49.0 and trunk versions of GDAL and libgeotiff.

Next, create a build directory and call `ccmake` to enter a graphical user interface for changing cmake variables. Alternatively, you can also set the variables on the command line.

```
$ mkdir build
$ cd build
$ ccmake ..
```

**Note:** Make sure that `WITH_GDAL` and `WITH_GEOTIFF` are set to `ON` (default is `OFF`). Hitting `'c'` followed by `'g'` will configure and generate all files for the build process. Running

```
$ make
$ make install
```

will build libLAS and install it on your system.

### Download and build the LAS Plugin

With ParaView and libLAS compiled, we are ready to build the plugin. First, obtain the [source](https://github.com/chambbj/paraview_las_plugin) from GitHub. Then, create a `Build` directory and run `ccmake`, i.e.,

```
$ mkdir Build
$ cd Build
$ ccmake ..
```

Make sure to set the ParaView_DIR cmake variable to

```
$ ParaView_DIR <FullPathToParaViewSource>/ParaView-3.14.1-Source/Build
```

Hit `'c'` and `'g'` to configurate and generate. Alternatively, you can set all on the command line by

```
$ cmake -DParaView_DIR=<FullPathToParaViewSource>/ParaView-3.14.1-Source/Build ..
```

Running

```
$ make
```

will compile the plugin and make it available for use within ParaView.

## Importing the LAS Plugin in ParaView

Now that we have built the plugin, we are ready to import it in ParaView. First, start ParaView

```
$ cd <FullPathToParaViewSource>/ParaView-3.14.1-Source/Build
$ bin/paraview
```

Then, go to **Tools/Manage Plugins**. Press the **Load New...** button and navigate to ParaView's `ParaView/Build/bin` directory. Navigate to the `Build/bin` directory where you unpacked the plugin and select **libvtkLASReader.so** to load. You might also want to enable ***Auto Load*** so that the plugin gets automatically loaded. You should now be ready to use the LAS Plugin in ParaView.
