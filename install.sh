#!/bin/sh

(cd lib/ && sudo make install) && (cd bin/ && sudo make install)
