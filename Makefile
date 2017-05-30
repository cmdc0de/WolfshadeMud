all:
	$(MAKE) -C src

clean:
	$(MAKE) clean -C src

wolfshade:
	$(MAKE) wolfshade -C src
