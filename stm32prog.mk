##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=stm32prog
ConfigurationName      :=Debug
WorkspacePath          :=/home/lieven/workspace
ProjectPath            :=/home/lieven/workspace/stm32prog
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Lieven
Date                   :=19/05/19
CodeLitePath           :=/home/lieven/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="stm32prog.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -pthread -lrt
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)./src $(IncludeSwitch)../microAkka/src $(IncludeSwitch)../Common $(IncludeSwitch)../ArduinoJson $(IncludeSwitch)../paho.mqtt.c/src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)Common $(LibrarySwitch)paho-mqtt3a-static 
ArLibs                 :=  "Common" "libpaho-mqtt3a-static" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../Common/Debug $(LibraryPathSwitch)../paho.mqtt.c/src 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++11 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_Programmer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Keyboard.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_Programmer.cpp$(ObjectSuffix): src/Programmer.cpp $(IntermediateDirectory)/src_Programmer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/lieven/workspace/stm32prog/src/Programmer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Programmer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Programmer.cpp$(DependSuffix): src/Programmer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Programmer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Programmer.cpp$(DependSuffix) -MM src/Programmer.cpp

$(IntermediateDirectory)/src_Programmer.cpp$(PreprocessSuffix): src/Programmer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Programmer.cpp$(PreprocessSuffix) src/Programmer.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/lieven/workspace/stm32prog/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_Keyboard.cpp$(ObjectSuffix): src/Keyboard.cpp $(IntermediateDirectory)/src_Keyboard.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/lieven/workspace/stm32prog/src/Keyboard.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Keyboard.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Keyboard.cpp$(DependSuffix): src/Keyboard.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Keyboard.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Keyboard.cpp$(DependSuffix) -MM src/Keyboard.cpp

$(IntermediateDirectory)/src_Keyboard.cpp$(PreprocessSuffix): src/Keyboard.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Keyboard.cpp$(PreprocessSuffix) src/Keyboard.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


