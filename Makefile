# Inspired by https://github.com/archlinux/arch-security-tracker/blob/master/Makefile

PYTEST?=py.test
PYTEST_OPTIONS+=-s
PYTEST_INPUT?=test
PYTEST_COVERAGE_OPTIONS+=--cov-report=term-missing --cov-report=html:test/coverage --cov=pycman
EXT_COVERAGE_DIR=test/ext-coverage
BUILD_DIR=build/lib.linux-x86_64-3.7

.PHONY: test

build:
	$(PYTHON) setup.py build

test: test-py

test-py coverage:
	PYTHONPATH="${BUILD_DIR}:.:${PYTHONPATH}" ${PYTEST} ${PYTEST_INPUT} ${PYTEST_OPTIONS} ${PYTEST_COVERAGE_OPTIONS}

open-coverage: coverage
	${BROWSER} test/coverage/index.html

ext-coverage:
	LDFLAGS="-fprofile-arcs -ftest-coverage" CFLAGS="-fprofile-arcs -ftest-coverage" python setup.py build
	PYTHONPATH="${BUILD_DIR}:.:${PYTHONPATH}" ${PYTEST} ${PYTEST_INPUT} ${PYTEST_OPTIONS}

	test -d ${EXT_COVERAGE_DIR} || mkdir ${EXT_COVERAGE_DIR}
	gcovr -r . --html -o ${EXT_COVERAGE_DIR}/index.html --html-details --html-title="pyalpm C coverage"

open-ext-coverage: ext-coverage
	${BROWSER} ${EXT_COVERAGE_DIR}/index.html

.PHONY: clean
clean:
	-rm -r build
