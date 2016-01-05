CFLAGS		= -std=c++11 -I../
OBJECTS		= cpptempl_test.o

test : $(OBJECTS)
	g++ -I./ $(OBJECTS) $(CFLAGS) -o test



%.o: %.cc
	g++ $(CFLAGS) $(INCLUDE) -c -o $@ $<

-include $(OBJECTS:.o=.d) # $(OBJECTS.o=.d)replace all *.o to *.d

%.d: %.cc
	set -e; rm -f $@; \
	g++ -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	-rm *.o *.d
	-rm *.d.*
