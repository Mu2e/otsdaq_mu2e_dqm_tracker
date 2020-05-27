Here is a bit of information about this Repo and its use:

# Log in From on mu2edaq01

```
$ ssh -X mu2edaq13
$ cd ~test_stand/ots_agg
$ source setup_ots.sh
$ mz 
```
# Create the directory from this Repo
```
$ create repo via: 
$ UpdateOTS.sh  --pullall 
$ source ots_get_and_fix_repo.sh otsdaq-mu2e-dqm
$ mz
```

# Too push code to this repo:
```
$ git status
$ git add NEW_STUFF
$ git commit -m "Information about new stuff"
$ git push origin develop (develop is branch name here)
```
# To Clone this repo
```
$ mrb gitCheckout -d mu2e_otsdaq_dqm http://cdcvs.fnal.gov/projects/mu2e-otsdaq-dqm
```
# Modify CMake files in order to compile
```
$ cp CMakeLists.txt 
```
from mu2e trigger directory. Will need to add in similar files to each subdirectory. Check for places we might need to change things.

Also remember to add in the new directory to the Head CMakeList.txt in srcs:

```
$ cd YOUR_TOP_DIRECTORY_NAME/srcs
$ EDITOR_NAME CMakeList.txt
```
Copy another package block and change the name :

```
set(otsdaq_mu2e_dqm_not_in_ups true)
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/otsdaq_mu2e_dqm )
include_directories ( $ENV{MRB_BUILDDIR}/otsdaq_mu2e_dqm )

```
# Clean Build:
```
$ mz
```

# To Use this Repo to make your own ARTMODULE with Vizualizer:

- The example I created is inside ArtModule - it is called Prototype module.
- In that module you will see how we connect to the TCP. Here only 1 TH1F is filled and Broadcast.
- The Histograms have there own class - ProtoTypeHist - in there you have to book and initiate all the RootObjects.
- The Consumer for this example is in DataProcessorPlugins -  see dqmMu2eHistoConsumer_processor.cc to see how the consumer reads the root object using only the name. You must have a class member of the same type as you filled in you module. There is o need to rename or rebook (that's why we have the class). 
- To add in Mu2e DataProducts you need to make sure you add in the Mu2e libraries- see ArtModule/CMakeList for examples.
