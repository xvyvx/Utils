include_guard(DIRECTORY)

macro(ParseStructuredLevelParameters keywordPrefix paramLevelPrefix currentUnparsedParams)
    if(${keywordPrefix}_Keywords)
        ParseStructuredLevelParametersCore(${keywordPrefix} ${paramLevelPrefix} ${currentUnparsedParams})
    endif()
endmacro()

macro(DefineIndexedKeywordGroup keywordPrefix paramLevelPrefix attr)
    set(currentParamAttrKey ${attr})
    if(DEFINED ${paramLevelPrefix}_${attr}_index)
        math(EXPR ${paramLevelPrefix}_${attr}_index "${${paramLevelPrefix}_${attr}_index}+1")
    else()
        set(${paramLevelPrefix}_${attr}_index 0)
    endif()
    list(APPEND ${paramLevelPrefix}_${attr}_${${paramLevelPrefix}_${attr}_index} 1)
    list(POP_BACK ${paramLevelPrefix}_${attr}_${${paramLevelPrefix}_${attr}_index})
    list(APPEND ${paramLevelPrefix}_All_Param_Names ${paramLevelPrefix}_${attr}_${${paramLevelPrefix}_${attr}_index})
    list(APPEND ${keywordPrefix}_All_Param_Keys ${attr})
endmacro()

macro(ParseStructuredLevelParametersCore keywordPrefix paramLevelPrefix)
    set(currentUnparsedParams ${ARGN})
    DefineIndexedKeywordGroup(${keywordPrefix} ${paramLevelPrefix} "Default")
    foreach(param ${currentUnparsedParams})
        list(FIND ${keywordPrefix}_Keywords ${param} tempKeywordIndex)
        if(NOT tempKeywordIndex EQUAL -1)
            list(GET ${keywordPrefix}_Keywords ${tempKeywordIndex} keyword)
            DefineIndexedKeywordGroup(${keywordPrefix} ${paramLevelPrefix} "${keyword}")
        else()
            list(APPEND ${paramLevelPrefix}_${currentParamAttrKey}_${${paramLevelPrefix}_${currentParamAttrKey}_index}
                ${param})
        endif()
        list(POP_FRONT currentUnparsedParams)
    endforeach()
    foreach(tempParamKey tempParamName IN ZIP_LISTS ${keywordPrefix}_All_Param_Keys
        ${paramLevelPrefix}_All_Param_Names)
        ParseStructuredLevelParameters(${keywordPrefix}_${tempParamKey} ${tempParamName} "${${tempParamName}}")
        unset(${paramLevelPrefix}_${tempParamKey}_All_Param_Names)
        unset(${keywordPrefix}_${tempParamKey}_All_Param_Keys)
    endforeach()
endmacro()