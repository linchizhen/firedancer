$(call add-hdrs,fd_smallset.c fd_set.c fd_sort.c fd_map.c fd_prq.c)
$(call make-unit-test,test_smallset,test_smallset,fd_util)
$(call make-unit-test,test_set,test_set,fd_util)
$(call make-unit-test,test_sort,test_sort,fd_util)
$(call make-unit-test,test_map,test_map,fd_util)
$(call make-unit-test,test_prq,test_prq,fd_util)
