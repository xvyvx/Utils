include_guard(DIRECTORY)

include(${CMAKE_CURRENT_LIST_DIR}/../Common/ParamHelper.cmake)

function(AddTestCaseToTarget targetName)
    set(Root_Keywords "TESTCASE")
    set(Root_TESTCASE_Keywords "FILTER" "COND")
    ParseStructuredLevelParameters("Root" "Root" "${ARGN}")
    foreach(setValueIndex RANGE 0 ${Root_TESTCASE_index})
        if((NOT DEFINED Root_TESTCASE_${setValueIndex}_COND_0
                AND DEFINED Root_TESTCASE_${setValueIndex}_Default_0)
            OR "${Root_TESTCASE_${setValueIndex}_COND_0}")
            add_test(NAME ${Root_TESTCASE_${setValueIndex}_Default_0}
                COMMAND ${targetName} --run_test=${Root_TESTCASE_${setValueIndex}_FILTER_0} --log_level=message)
            set_tests_properties(${Root_TESTCASE_${setValueIndex}_Default_0} PROPERTIES PASS_REGULAR_EXPRESSION
                "No errors detected")
        endif()
    endforeach()
endfunction()