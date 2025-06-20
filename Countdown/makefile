bin=countdown
src=countdown.c

$(bin):$(src)
	@gcc -o $@ $^
	@echo "compiler $(src) to $(bin).."
.PHONY:clean
clean:
	@rm -f $(bin)
	@echo "clean project..."
	