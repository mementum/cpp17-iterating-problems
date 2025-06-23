###############################################################################
# Specifics for this Makefile
###############################################################################
# Default language
blang ?= en

# build only specific chapters
buildchap ?=

# Define things that need to be grepped out when using the md/ad/mk processors
# which may be executing external programs which do output undesired things
GREP_OUT_MD :=
GREP_OUT_AD :=
GREP_OUT_MK :=

# For mkdocs targets: numbering that makes the cut [half-open-range)
SRC_MK_0 := 0007
SRC_MK_9 := 9999
MK_TAB_MAIN := "Book"
MK_TAB_PDF := "PDF Downloads"

###############################################################################
# DO NOT EDIT BELOW THIS LINE ... Unless you really need to
###############################################################################
# Force shell for pipefile with grep
SHELL := /bin/bash

###############################################################################
# BASE DIRECTORIES
###############################################################################
SRC_BASE := src
SRC_EXTRA := 00 zz

###############################################################################
# LANGUAGE TARGET - Default if not given as argument
###############################################################################
# lowercase for the command line - uppercase internally
BLANG := $(blang)

# Find out which languages are present to automatically generate all targets
MAYBEBLANGS := $(patsubst $(SRC_BASE)/%, %,$(wildcard $(SRC_BASE)/*))
MAYBEBLANGS := $(filter-out $(SRC_EXTRA), $(MAYBEBLANGS))
# Languages not ending in "-" are "enabled" and will be considered for a build
BLANGS := $(shell echo $(MAYBEBLANGS) | tr ' ' '\n' | grep -E '.+[^-]$$')
# Languages ending in "-" are "disabled" and will not be considered for a build
XBLANGS := $(shell echo $(MAYBEBLANGS) | tr ' ' '\n' | grep -vE '.+[^-]$$')

# If there is only one language set it as the default
LANG_FIRST := $(firstword $(BLANGS))
LANG_LAST := $(lastword $(BLANGS))
# if first and last and the same ... it is the one and only
ifeq ($(LANG_FIRST),$(LANG_LAST))
BLANG := $(LANG_FIRST)
endif

# Let's see if a default language was specified or found out
# if no build language was determined ...
# if one the languages in the dir has the prefix "+" it is the default
ifeq ($(strip $(BLANG)),)
BLANG := $(shell echo $(BLANGS) | tr ' ' '\n' | grep -E '.+[+]$$')
endif

# if still empty
ifeq ($(strip $(BLANG)),)
  # try the all-xx make goals to pass this state
  # the all-xx will then rekick make with the "xx" suffix
  ALL_LANGS := $(filter all-%,$(MAKECMDGOALS))
  ALL_LANGS := $(subst all-,,$(ALL_LANGS))
  BLANG := $(firstword $(ALL_LANGS))
  ifeq ($(BLANG),langs)
    # if it is all-langs, use any of them as target, to pass this stage
    # the all-langs target will then do each language setting BLANG
    BLANG := $(firstword $(BLANGS))
  endif
  # it was blank, after several attempts and still blank, cannot proceed
  ifeq ($(strip $(BLANG)),)
    $(error Multiple languages, no language requested and default marked with "+")
  endif
endif

# the language specified via "blang=xx" or "all-xx" may not actually exist
# check it
BLANG_IN_LANGS := $(findstring $(BLANG),$(BLANGS))
ifeq ($(strip $(BLANG_IN_LANGS)),)
  # see if adding a "+" finds it, as it may be marked as default on disk
  BLANGPLUS_IN_LANGS := $(findstring $(BLANG)+,$(BLANGS))
  ifneq ($(strip $(BLANGPLUS_IN_LANGS)),)
    BLANG := $(BLANGPLUS_IN_LANGS)
  else
    # with/out +, not one of the available languages, bail out
    $(error $(BLANG) not available as language)
  endif
endif

# target language ... in case BLANG has a + at the end
TLANG := $(shell echo $(BLANG) | tr -d '+')

# export BLANG and TLANG for scripts. TLANG should be used
export BLANG
export TLANG

###############################################################################
# OTHER  DIRECTORIES
###############################################################################
# source .md
# Order for files in sudirectories: 00 lan zz
# Files with the same name (no extension) will be merged.
# Those in 00 will come first and those in zz will come last.
# 00 and zz language independent directives/content
# SRC_DIRS := $(foreach dir,$(BLANG) $(SRC_EXTRA),$(SRC_BASE)/$(dir))

# The above generate the dirs, but leaves no trace of the individual
# dirs making up the src_dirs and after time they are needed
SRC_LANG := $(SRC_BASE)/$(BLANG)
SRC_00 := $(SRC_BASE)/$(firstword $(SRC_EXTRA))
SRC_ZZ := $(SRC_BASE)/$(lastword $(SRC_EXTRA))
SRC_EXTRAS := $(SRC_00) $(SRC_ZZ)

SRC_DIRS := $(SRC_00) $(SRC_LANG) $(SRC_ZZ)

# EXTENDERS - ruby plugins for asciidoctor
EXTENDERS_DIR := extenders

# FONTS
FONTS_DIR := fonts

# source images
IMG_DIR := images
IMG_DIR_LANG := images/$(TLANG)
IMG_DIRS := $(IMG_DIR) $(IMG_DIR_LANG)

# source covers
COVER_DIR := $(IMG_DIR)/covers/$(TLANG)

# source themes
THEME_DIR := theme

# base name for build directories
BASE_BUILD_DIR := build

# intermediate md processed files
BUILD_DIR_MDX := $(BASE_BUILD_DIR)/mdx/$(TLANG)

# intermediate ad processed files from kramdoc
BUILD_DIR_ADX := $(BASE_BUILD_DIR)/adx/$(TLANG)

# output book-md
BUILD_DIR_MD := $(BASE_BUILD_DIR)/md/$(TLANG)

# output book-adoc
BUILD_DIR_ADOC := $(BASE_BUILD_DIR)/adoc/$(TLANG)

# output mkdocs files
YML_EXT := .yml

BUILD_DIR_MKDOCS := $(BASE_BUILD_DIR)/mkdocs/$(TLANG)
MKDOCS_DIR := mkdocs
MKDOCS_YML := $(MKDOCS_DIR)/mkdocs$(YML_EXT)
MKDOCS_YML_TEMPLATE := $(MKDOCS_YML).template
MKDOCS_DOCS_DIR := $(MKDOCS_DIR)/docs
MKDOCS_CSS_DIR := $(MKDOCS_DOCS_DIR)/stylesheets
MKDOCS_JS_DIR := $(MKDOCS_DOCS_DIR)/js

# output directory for pdf/epub/html books
BUILD_DIR_BOOK := $(BASE_BUILD_DIR)/books/$(TLANG)

# EPUB temporary directory, as it may need fonts, styles and other added/updated
# For zip update, to have everything under this directory temporarily
EPUB_TMP := EPUB

###############################################################################
# ISBN
###############################################################################
# if isbn is passed as a target, final target names get an ISBN suffif
# and the back cover is the backcover with the isbn
ifeq (isbn,$(filter isbn,$(MAKECMDGOALS)))
ISBN := -isbn

isbn: ;
endif

###############################################################################
# THEME
###############################################################################
THEME_NAME := generic
THEME_FILE := $(THEME_DIR)/theme-$(THEME_NAME)$(YML_EXT)
THEME_FILE_SCREEN := $(wildcard $(THEME_DIR)/theme-$(THEME_NAME)-screen$(YML_EXT))
ifeq ($(strip $(THEME_FILE_SCREEN)),)
	THEME_FILE_SCREEN := $(THEME_FILE)
endif
THEME_EPUB := $(THEME_DIR)/epub

###############################################################################
# COVER DIMENSIONS
###############################################################################
COVER_THEME_RE := 's/.+resolution:\s+([0-9]+)x([0-9]+)/\1 \2/p'

COVER_RESOLUTION := $(shell sed -nr $(COVER_THEME_RE) $(THEME_FILE) | head -n 1)
COVER_WIDTH := $(firstword $(COVER_RESOLUTION))
COVER_HEIGHT := $(lastword $(COVER_RESOLUTION))

# For epub ratio  5/8 (0.625 or 1:1.16)
# Nothing prevents using the values from above, the 1:1.6 ratio is the
# recommended ideal by KDP
COVER_EPUB_WIDTH := 1600
COVER_EPUB_HEIGHT := 2560

# For epub ratio  5/8 (0.625 or 1:1.16)
# Nothing prevents using the values from above, the 1:1.6 ratio is the
# recommended ideal by KDP
COVER_EPUB2_WIDTH := 1400
COVER_EPUB2_HEIGHT := 2240

###############################################################################
# SOURCES
###############################################################################
# All source files. adoc are before markdown and 00 before source, zz, after
# need to pass it through tr to have lines and not a line

# Source files markdown
SRCFILES_MD := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.md))
SRCFILES_MD := $(shell echo $(SRCFILES_MD) | tr ' ' '\n' | sort -t '/' -k3)

# Source files asciidoc
SRCFILES_AD := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.adoc))
SRCFILES_AD := $(shell echo $(SRCFILES_AD) | tr ' ' '\n' | sort -t '/' -k3)

# Source files mkdocs
# Use the MD sourcefiles, restricted to the numbering. For awk we need the basename
# SRCFILES_MK0 := $(foreach x,$(SRCFILES_MD),$(notdir $(x)))
SRCFILES_MK0 := $(notdir $(SRCFILES_MD))
SRCFILES_MK0 := $(shell echo $(SRCFILES_MK0) | tr ' ' '\n' |  awk -v FS=- '$$1 >= $(SRC_MK_0) && $$1 <= $(SRC_MK_9) {print}')
SRCFILES_MK := $(foreach x,$(SRCFILES_MK0),$(filter %$(x),$(SRCFILES_MD)))

# Join adoc and markdown files
SRCFILES := $(SRCFILES_AD) $(SRCFILES_MD)
SRCFILES := $(shell echo $(SRCFILES) | tr ' ' '\n' | sort -t '/' -k3)

# document header
ADOC_HPREFIX := 0000
ADOC_HEADERS := $(shell echo $(SRCFILES) | tr ' ' '\n' | grep $(ADOC_HPREFIX))
ADOC_HEADER := $(firstword $(ADOC_HEADERS))

# build only specific chapters - filter them out and join with adoc headers
ifneq ($(strip $(buildchap)),)
BUILDCHAPS := $(foreach chap,$(buildchap),$(shell echo $(SRCFILES) | tr ' ' '\n' | grep -E '$(chap)'))
BUILDCHAPS := $(ADOC_HEADERS) $(BUILDCHAPS)
SRCFILES := $(BUILDCHAPS)
endif

# document covers
BACKCOVER_TITLE := $(shell sed -nE '/^:backcover-title:/ s/^:[^:]+:[ ]+//p' $(ADOC_HEADER))
BACKCOVER_TITLE := $(strip $(BACKCOVER_TITLE))

COVER_FRONT := $(COVER_DIR)/cover-$(COVER_WIDTH)x$(COVER_HEIGHT)-front.png
COVER_BACK := $(COVER_DIR)/cover-$(COVER_WIDTH)x$(COVER_HEIGHT)-back$(ISBN).png

COVER_EPUB_FRONT := $(COVER_DIR)/cover-$(COVER_EPUB_WIDTH)x$(COVER_EPUB_HEIGHT)-front.png
COVER_EPUB_BACK := $(COVER_DIR)/cover-$(COVER_EPUB_WIDTH)x$(COVER_EPUB_HEIGHT)-back.png

COVER_EPUB_FRONT_TIFF := $(basename $(COVER_EPUB_FRONT)).tiff

COVERS_EPUB_KDP := $(COVER_EPUB_FRONT_TIFF) $(COVER_EPUB_BACK)

COVER_EPUB2_FRONT := $(COVER_DIR)/cover-$(COVER_EPUB2_WIDTH)x$(COVER_EPUB2_HEIGHT)-front.png
COVER_EPUB2_BACK := $(COVER_DIR)/cover-$(COVER_EPUB2_WIDTH)x$(COVER_EPUB2_HEIGHT)-back.png

COVERS := $(COVER_FRONT) $(COVER_BACK)
COVERS_EPUB := $(COVER_EPUB_FRONT) $(COVER_EPUB_BACK)
COVERS_EPUB2 := $(COVER_EPUB2_FRONT) $(COVER_EPUB2_BACK)

# document images
IMG_EXT := png jpg jpeg tiff svg
IMGFILES := $(foreach ext,$(IMG_EXT),$(foreach idir,$(IMG_DIRS),$(wildcard $(idir)/*.$(ext))))
IMGFILES := $(strip $(IMGFILES))

# extenders
EXTENDERS_RB := $(wildcard $(EXTENDERS_DIR)/*.rb)

# Final possible sources/dependencies
FONTS := $(wildcard $(FONTS_DIR)/*.ttf $(FONTS_DIR)*.otf)
EPUB_STYLES := $(wildcard $(THEME_EPUB)/styles/*)

###############################################################################
# BUILD TARGET NAMES
###############################################################################
# Read version
BVERSION := $(file < version)

# Gather information from adoc_header
BAUTHOR := $(shell cat $(ADOC_HEADER) | sed -nE '/^:author:/s/:author:[ \t]+//p')
BREVDATE := $(shell cat $(ADOC_HEADER) | sed -nE '/^:revdate:/s/:revdate:[ \t]+//p')

# generate basename from book title
BHEADER := $(shell head -n 1 $(ADOC_HEADER))

# Keep a reference to the title
# extra $ to quote the EOLD $
# strip leading "= ", "remove everything from " :" until EOLD
BTITLE := $(shell echo "$(BHEADER)" | sed -E 's/^=[ \t]+//;s/[ \ŧ]+:.*$$//')

# Keep a reference to the subtitle
# Remove everything from ^= up to to " : ", keep the rest
BSTITLE := $(shell echo "$(BHEADER)" | sed -E 's/^=[^:]+:[ \t]+//')

# build a bookname that is filesystem friendly (and human friendly too)
# Remove any " - " sequence used to separate wording
BNAME := $(shell echo "$(BTITLE)" | sed 's/ - / /g')

# Remove any "''" sequence used to break the title
BNAME := $(shell echo "$(BNAME)" | sed "s/'//g")

# lowercase and change " " to "-"
BNAME := $(shell echo "$(BNAME)" | tr '[:upper:]' '[:lower:]' | tr ' ' '-')

# Final Book targets (adoc, md, pdf, epub, html)
BOOK_NAME := $(BNAME)-$(BVERSION)-$(TLANG)

# md book target
DEFAULTS_MD := $(BUILD_DIR_MD)/$(BOOK_NAME).yaml

BOOK_MD := $(BUILD_DIR_MD)/$(BOOK_NAME).md
BOOK_MD_EPUB2 := $(basename $(BOOK_MD))-epub2$(ISBN)$(suffix $(BOOK_MD))

# Intermediate targets
BOOK_MDX := $(BUILD_DIR_MDX)/$(BOOK_NAME).mdx
BOOK_ADX := $(BUILD_DIR_ADX)/$(BOOK_NAME).adx

# Ascii Book targets
BOOK_ADOC := $(BUILD_DIR_ADOC)/$(BOOK_NAME).adoc
BOOK_ADOC_EPUB := $(basename $(BOOK_ADOC))-epub$(ISBN)$(suffix $(BOOK_ADOC))

# mkdocs dependencies
MKDOCS_XTRA_SRCS := $(wildcard $(MKDOCS_DOCS_DIR)/*.md)
MKDOCS_XTRA_SRCS := $(MKDOCS_XTRA_SRCS) $(wildcard $(MKDOCS_CSS_DIR)/*.css)
MKDOCS_XTRA_SRCS := $(MKDOCS_XTRA_SRCS) $(wildcard $(MKDOCS_JS_DIR)/*.js)

MKDOCS_XTRA_DEPS := $(subst $(MKDOCS_DOCS_DIR),$(BUILD_DIR_MKDOCS),$(MKDOCS_XTRA_SRCS))

# File patched with links
PDF_DOWN_SRC := $(MKDOCS_DOCS_DIR)/pdf-downloads.md
PDF_DOWN_DST := $(BUILD_DIR_MKDOCS)/$(notdir $(PDF_DOWN_SRC))

# Mkdocs "book" targets
BOOK_MKDOCS := $(foreach x,$(notdir $(SRCFILES_MK)),$(BUILD_DIR_MKDOCS)/$(x))

# File that will be the index: source and target
MKDOCS_INDEX_MD := $(filter %index.md,$(MKDOCS_XTRA_SRCS))
BUILD_MKDOCS_INDEX_MD := $(BUILD_DIR_MKDOCS)/$(notdir $(MKDOCS_INDEX_MD))

# Final Targets
BOOK_NAME_FINAL := $(BUILD_DIR_BOOK)/$(BOOK_NAME)
BOOK_NAME_FINAL_EXTRA := $(BOOK_NAME_FINAL)$(ISBN)

# PDF Book targets
PDF := $(BOOK_NAME_FINAL_EXTRA).pdf
PDF_SCREEN := $(basename $(PDF))-screen$(suffix $(PDF))
PDF_PREPRESS := $(basename $(PDF))-prepress$(suffix $(PDF))

# There is no ISBN in the KDP Version
PDF_NOISBN := $(BOOK_NAME_FINAL).pdf
PDF_KDP := $(basename $(PDF_NOISBN))-kdp$(suffix $(PDF))

OPTIM := -opt
PDF_SCREEN_OPT := $(basename $(PDF_SCREEN))$(OPTIM)$(suffix $(PDF_SCREEN))
PDF_PREPRESS_OPT := $(basename $(PDF_PREPRESS))$(OPTIM)$(suffix $(PDF_PREPRESS))
PDF_KDP_OPT := $(basename $(PDF_KDP))$(OPTIM)$(suffix $(PDF_KDP))

# EPUB Book targets - No ISBN
EPUB := $(BOOK_NAME_FINAL).epub
EPUB_KDP := $(basename $(EPUB))-kdp$(suffix $(EPUB))

EPUB2 := $(basename $(EPUB))-epub2$(suffix $(EPUB))
EPUB2_KDP := $(basename $(EPUB2))-kdp$(suffix $(EPUB2))

# HTML Book targets
HTML := $(BOOK_NAME_FINAL).html
HTML_ZIP := $(basename $(HTML))-zip$(suffix $(HTML)).zip
HTML_TEST := $(basename $(HTML))-test$(suffix $(HTML))

# Docbook Book targets
DOCBOOK := $(BOOK_NAME_FINAL).xml

# Github repo and links
GH_REPO := $(shell cat $(ADOC_HEADERS) | sed -nE '/^:repo_url:.*/ s/^:repo_url:[ ]+//p')
###############################################################################
# TOOLING DEFINITION
###############################################################################
# internal tooling
# for the bookfix tool
BOOKFIX := ./bin/book-mdfix.py
FIX_MD2MK := ./bin/fix-md2mk.py
FIX_MD2AD := ./bin/fix-md2ad.py
FIX_AD2AD := ./bin/fix-ad2ad.py
BF_FIX_EPUB_DATE := --pfix-epub-date
BF_PDEFAULTS := --pdefaults $(DEFAULTS_MD)

# Check if python env/package manager is present. if not mkdocs will be tested
# without it
PDM := pdm
ifneq ($(PDM),)
  PDM := $(notdir $(shell which $(PDM) 2>/dev/null))
endif

PDM_RUN := cd $(MKDOCS_DIR) &&
ifneq ($(PDM),)
  PDM_RUN := $(PDM_RUN) $(PDM) run
endif

# to build mkdocs
MKDOCS := mkdocs
MKDOCS_SERVE := $(MKDOCS) serve
MKDOCS_BSERVE := $(MKDOCS_SERVE) &
MKDOCS_BUILD := $(MKDOCS) build

PYTHON3 := python3

# convert md to asciidoc
KRAMDOC := kramdoc
# Done by fix-md.py now to control parts
# KRAMDOC_OPTS := --heading-offset=1
KRAMDOC_OPTS :=

# for epub2 generation with pandoc
PANDOC := pandoc
PD_MD_2_EPUB2 := -f markdown -t epub2
PD_TOP_LVL_DIV := --top-level-division=chapter
PD_DEFAULTS := --defaults $(DEFAULTS_MD)
PD_COV_FRONT_EPUB2 := --epub-cover-image=$(COVER_EPUB2_FRONT)

# see PDF files using the makefile
PDF_EVINCE := evince
PATH_EVINCE := $(shell which $(PDF_EVINCE) 2>/dev/null)

# to automatically resize cover images
CONVERT := convert

# For various packing operations with HTML and EPUB formats
ZIP := zip

# Kill process by name (procps package)
PKILL := pkill

# GitHub Management
GHCLI := gh

###############################################################################
# ASCIDOCTOR DEFINITIONS
###############################################################################
# tools and basic options
ADOCTOR := asciidoctor
ADOCTORPDF := asciidoctor-pdf
ADOCTOREPUB := asciidoctor-epub3

# to optimize
HEXAPDF := hexapdf

# basic options for output and image location
# delayed expansion to expand when target is known
ADOCTOR_DSTFILE = --out-file $@
ADOCTOR_DSTFILE_CURDIR = --out-file $(notdir $@)

ADOCTOR_PDF_THEME := -a pdf-theme=$(THEME_FILE)
ADOCTOR_PDF_FONTS := -a pdf-fontsdir="$(FONTS_DIR);GEM_FONTS_DIR"
ADOCTOR_PDF_EXTENDERS := $(addprefix -r ./,$(EXTENDERS_RB))

# target build into (ex.) build/book/en/dstfile and images are at build level
# hence the need to dereference the image. Same below for the covers
ADOCTOR_IMGDIR := -a imagesdir=../../..

# pdf presentation options
ADOCPDF_SCREEN_OPTS := -a media=screen
ADOCPDF_PREPRESS_OPTS := -a media=prepress
ADOCPDF_EXTRA_OPTS := $(PDF_EXTRA_OPTS)

###############################################################################
# COVER for PDF
###############################################################################
# front/back-cover attribute value
COVER_EMPTY_ATTR := ~
# with [fit=fill] if the image has the same aspect ratio as the book size: ok
# if it is close enough, the distortion will be "acceptable". Else rework cover
COVER_FRONT_ATTR := image:$(COVER_FRONT)[fit=fill]
COVER_BACK_ATTR := image:$(COVER_BACK)[fit=fill]

# front/back-cover attribute options for PDF
COVER_FRONT_OPTS := -a 'front-cover-image=$(COVER_FRONT_ATTR)'
COVER_BACK_OPTS := -a 'back-cover-image=$(COVER_BACK_ATTR)'
COVER_OPTS := $(COVER_FRONT_OPTS) $(COVER_BACK_OPTS)

# Empty front cover for prepress in kdp mode to keep recto/verso page order
COVER_EMPTY_FRONT_OPTS := -a 'front-cover-image=$(COVER_EMPTY_ATTR)'
COVER_EMPTY_BACK_OPTS :=
COVER_EMPTY_OPTS := $(COVER_EMPTY_FRONT_OPTS) $(COVER_EMPTY_BACK_OPTS)

###############################################################################
# COVER for EPUB
###############################################################################
IMG_ATTR := image:../../..
COVER_EPUB_SIZE := $(COVER_EPUB_WIDTH), $(COVER_EPUB_HEIGHT)
COVER_FRONT_EPUB_ATTR := $(IMG_ATTR)/$(COVER_EPUB_FRONT)[Front Cover, $(COVER_EPUB_SIZE)]
COVER_BACK_EPUB_ATTR := $(IMG_ATTR/$(COVER_EPUB_BACK)[Back Cover, $(COVER_EPUB_SIZE)]

# front/back-cover attribute options for epub
COVER_FRONT_EPUB_OPTS := -a 'front-cover-image=$(COVER_FRONT_EPUB_ATTR)'
COVER_BACK_EPUB_OPTS := -a 'back-cover-image=$(COVER_BACK_EPUB_ATTR)'
COVER_EPUB_OPTS := $(COVER_FRONT_EPUB_OPTS) $(COVER_BACK_EPUB_OPTS)

###############################################################################
# TARGETS - GENERAL
###############################################################################
# default target
mydef: help

# generate just a media screen version with cover
pdf: pdf-screen

# target to make all final books
all: pdf-screen pdf-prepress pdf-kdp epub epub-kdp epub2 epub2-kdp html docbook

# target to make all languages
all-langs:
	$(MAKE) $(foreach lang,$(BLANGS),all-$(lang))

# target to make all in a language ($* is that % has matched)
all-%:
	$(if $(findstring $*,$(BLANGS)),,$(error "$* not in available languages: $(BLANGS)"))
	$(MAKE) blang=$* all

langs:
	@echo "Default language: $(BLANG)"
	@echo "Languages enabled: $(BLANGS)"
	@echo "Languages disabled: $(XBLANGS)"

###############################################################################
# FUNCTIONS
###############################################################################
define echo_stage =
	@echo "========================================"
	@echo "===== CREATING $(1) ====="
	@echo "========================================"
endef

define makedir_dir =
	[ -d $(1) ] || mkdir -p $(1)
endef

define makedir_for_file =
	[ -d $(dir $(1)) ] || mkdir -p $(dir $(1))
endef

define makedir_for_file_and_move =
	$(call makedir_for_file,$@)
	mv $(notdir $@) $@
endef

define find_tool =
	$(eval tmptool := $(shell $(2) which $(1) 2>/dev/null))
	@[ -n "$(tmptool)" ] || echo "$(1): not found"
	@[ -z "$(tmptool)" ] || echo "$(1): found: $(tmptool)"
endef

define find_tool_or_exit =
	$(eval tmptool := $(shell $(2) which $(1) 2>/dev/null))
	@[ -n "$(tmptool)" ] || (echo "$(1): not found"; exit 1)
endef

# Exit status of "$(1)" is desired and not that of grep -v
# which will kill the execution if a line is filtered out (as wished)
# Title: "Get exit status of process that's piped to another"
# https://unix.stackexchange.com/a/70675
define grep_vx =
	(((($(1); echo $$? >&3) | grep -v$(3) $(2) >&4) 3>&1) | (read xs; exit $$xs)) 4>&1
endef

define grep_ev =
	$(call grep_vx,$(1),$(2),E)
endef

define grep_v =
	$(call grep_vx,$(1),$(2))
endef

###############################################################################
# TARGETS AND RULES
###############################################################################
###############################################################################
# INTERMEDIATE TARGET: markdown book, transformed adocs, adoc book
###############################################################################
# target to join all md files
book-md: $(BOOK_MD)

# rule to join all md files
$(BOOK_MD): $(ADOC_HEADERS) $(SRCFILES_MD)
	$(call echo_stage,book-md)
	$(call find_tool_or_exit,$(PYTHON3))
	$(call makedir_for_file,$@)
	$(BOOKFIX) $(BF_PDEFAULTS) -o $@ $^

# target to join all md files with a 1400 backcover
book-md-epub2: $(BOOK_MD_EPUB2)

# rule to join all md files
$(BOOK_MD_EPUB2): $(BOOK_MD)
	$(call echo_stage,book-md-epub2)
	cp $< $@
	echo "# $(BACKCOVER_TITLE) {.unnumbered}" >> $@
	echo "![.]($(COVER_EPUB2_BACK))" >> $@

# Function to generate the PDF Links (real or fake)
define mkdocs_pdf_down =
	cp $(PDF_DOWN_SRC) $(PDF_DOWN_DST)
	echo "[1]: $(1)" >> $(PDF_DOWN_DST)
	echo "[2]: $(2)" >> $(PDF_DOWN_DST)
	echo "  - $(MK_TAB_PDF): $(notdir $(PDF_DOWN_DST))" >> $(MKDOCS_YML)
endef

# make a complete mkdocs site with optimized PDF files
mkdocs-pdfopt: mkdocs $(PDF_SCREEN_OPT) $(PDF_PREPRESS_OPT)
	$(call echo_stage,mkdocs-pdfopt)
	$(eval src := $(filter-out $<,$^))
	$(eval dst := $(notdir $(subst $(OPTIM),,$(src))))
	$(foreach s,$(src),$(shell cp $(s) $(BUILD_DIR_MKDOCS)/$(notdir $(subst $(OPTIM),,$(s)))))
	$(call mkdocs_navigation,$(BOOK_MKDOCS))
	$(call mkdocs_pdf_down,$(firstword $(dst)),$(lastword $(dst)))

# make a complete mkdocs site with regular pdf files
mkdocs-pdf: mkdocs $(PDF_SCREEN) $(PDF_PREPRESS)
	$(call echo_stage,mkdocs-pdf)
	$(eval src := $(filter-out $<,$^))
	$(eval dst := $(notdir $(src)))
	cp $(src) $(BUILD_DIR_MKDOCS)
	$(call mkdocs_navigation,$(BOOK_MKDOCS))
	$(call mkdocs_pdf_down,$(firstword $(dst)),$(lastword $(dst)))

# make a complete mkdocs site with links to but no real pdf files
# i.e.: check the mkdocs run without waiting for PDF generation
mkdocs-fakepdf: mkdocs
	$(call echo_stage,mkdocs-fakepdf)
	$(call mkdocs_navigation,$(BOOK_MKDOCS))
	$(call mkdocs_pdf_down,$(PDF_SCREEN),$(PDF_PREPRESS))

# function to regenerate the first part of the navigation
# i.e. the book md files. Needed to later be able to cleanly
# add the pdf-download section
define mkdocs_navigation =
	$(eval tgts := $(notdir $(1)))
	$(eval tgts := $(notdir $(BUILD_MKDOCS_INDEX_MD)) $(tgts))
	$(eval tmpfile := $(shell mktemp --suffix=$(YML_EXT)))
	@# add the selected md files to the mkdocs yml config (skip template dependency)
	@# From mkdocs.yml template to file with navigation
	cp $(MKDOCS_YML_TEMPLATE) $(tmpfile)
	@# echo "" >> $(tmpfile)
	@# echo "exclude_docs: |" >> $(tmpfile)
	@# echo "  $(notdir $(MKDOCS_INDEX_MD))" >> $(tmpfile)
	echo "" >> $(tmpfile)
	echo "nav:" >> $(tmpfile)
	echo "  - $(MK_TAB_MAIN):" >> $(tmpfile)
	echo " $(tgts)" | sed 's/ /\n    - /g' | tail +2 >> $(tmpfile)
	cp $(tmpfile) $(MKDOCS_YML)
	-rm $(tmpfile)
endef

# Regenerate the mkdocs.yml file if any md source has changed
# or hte index is missing or the template for it has changed
$(MKDOCS_YML): $(MKDOCS_YML_TEMPLATE) $(BUILD_MKDOCS_INDEX_MD) $(BOOK_MKDOCS)
	$(call echo_stage,book-mkdocs)
	$(call mkdocs_navigation,$(BOOK_MKDOCS))

# target which generates the mkdocs content
# depands on the site definition (mkdocs.ym) and anything in the mkdocs/docs dir
mkdocs: $(MKDOCS_YML) $(MKDOCS_XTRA_DEPS)

# Serve a site
mk-serve: mkdocs-serve

# main serve recipe, it
mkdocs-serve: mkdocs
	$(call find_tool_or_exit,$(MKDOCS),$(PDM_RUN))
	$(PDM_RUN) $(MKDOCS) serve

# serve, but first killing it if running and serving in the background
mkdocs-bserve: mkdocs mkdocs-kill
	$(call find_tool_or_exit,$(MKDOCS),$(PDM_RUN))
	$(PDM_RUN) $(MKDOCS) serve &

# Build an mkdocs site
mk-build: mkdocs-build

mkdocs-build: mkdocs-pdfopt
	$(call find_tool_or_exit,$(MKDOCS),$(PDM_RUN))
	$(PDM_RUN) $(MKDOCS) build

# Deploy site
mk-deploy: mkdocs-deploy

mkdocs-deploy: mkdocs-pdfopt
	$(call find_tool_or_exit,$(MKDOCS),$(PDM_RUN))
	$(PDM_RUN) $(MKDOCS) gh-deploy --no-history

# kill a running mkdocs instance (probabl serving the site)
mk-kill: mkdocs-kill

mkdocs-kill:
	$(call find_tool_or_exit,$(PKILL))
	-$(PDM_RUN) $(PKILL) $(MKDOCS)

# General rule for anything coming from mkdocs_docs_dir
# it may need to generate directories to copy without error
$(BUILD_DIR_MKDOCS)/%: $(MKDOCS_DOCS_DIR)/%
	$(call makedir_for_file,$@)
	cp -r $< $@

# Rule to update index.md if the source changes
$(BUILD_MKDOCS_INDEX_MD): $(MKDOCS_INDEX_MD) $(ADOC_HEADER)
	$(call makedir_for_file,$@)
	$(FIX_MD2MK) -o $@ $(ADOC_HEADERS) $<
	sed -i -E '/\{:version:\}/ s/\{:version:\}/$(BVERSION)/' $@

# To consider the "00" and "zz" directories the content of those
# and the source md have to put together in a tmpfile, losing
# the ability to use $^ as the input for fix-md2mk
$(BUILD_DIR_MKDOCS)/%.md: $(ADOC_HEADERS) $(SRC_LANG)/%.md
	$(call echo_stage,mkdocs-$(notdir $@))
	$(call find_tool_or_exit,$(PYTHON3))
	$(call makedir_for_file,$@)
	$(eval tmpfile := $(shell mktemp --suffix=.md))
	$(eval mdbase := $(notdir $@))
	$(eval mdfiles := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/$(mdbase))))
ifeq ($(GREP_OUT_MK),)
	$(FIX_MD2MK) -o $@ $(ADOC_HEADERS) $(mdfiles)
else
	$(call grep_ev,$(FIX_MD2MK) -lo -o $@ $(ADOC_HEADERS) $(mdfiles),$(GREP_OUT_MK))
endif

# target which generates the book .adoc
book-adoc: $(BOOK_ADOC)

# Environment variables for processors
# $(BOOK_ADOC): export VARNAME = varvalue

# rule which generates the book .adoc
$(BOOK_ADOC): $(SRCFILES)
	$(call echo_stage,book-adoc)
	$(call find_tool_or_exit,$(PYTHON3))
	$(call find_tool_or_exit,$(KRAMDOC))
	$(call makedir_for_file,$@)
	$(call makedir_for_file,$(BOOK_MDX))
	$(call makedir_for_file,$(BOOK_ADX))
ifeq ($(GREP_OUT_MD),)
	$(FIX_MD2AD) -o $(BOOK_MDX) $^
else
	$(call grep_ev,$(FIX_MD2AD) -lo -o $(BOOK_MDX) $^,$(GREP_OUT_MD))
endif
	$(KRAMDOC) $(KRAMDOC_OPTS) --output=$(BOOK_ADX) $(BOOK_MDX)
ifeq ($(GREP_OUT_AD),)
	$(FIX_AD2AD) -o $@ $(ADOC_HEADERS) $(BOOK_ADX)
else
	$(call grep_ev,$(FIX_AD2AD) -lo -o $@ $(ADOC_HEADERS) $(BOOK_ADX),$(GREP_OUT_AD))
endif

# target which generates the book .adoc with a 1600 back-cover
book-adoc-epub: $(BOOK_ADOC_EPUB)

# rule which generates the book .adoc with a 1600 back cover
$(BOOK_ADOC_EPUB): $(BOOK_ADOC)
	$(call echo_stage,book-adoc-epub)
	cp $< $@
	echo >> $@
	echo "== $(BACKCOVER_TITLE)" >> $@
	echo >> $@
	echo "image::$(COVER_BACK)[.]" >> $@

###############################################################################
# TARGETS: optimize
###############################################################################
define pdf_opt =
	$(call echo_stage,$(1))
	$(call find_tool_or_exit,$(HEXAPDF))
	$(HEXAPDF) optimize --force --compress-pages $< $@
endef

# create dynamic opt targets if opt is part of the goals
ifeq (opt,$(filter opt,$(MAKECMDGOALS)))
ALL_PDF_TARGETS := $(filter pdf-%,$(MAKECMDGOALS))
PDF_TARGETS := $(filter-out opt,$(ALL_PDF_TARGETS))
opt: $(foreach ptarget,$(PDF_TARGETS),$(ptarget)$(OPTIM))
endif

###############################################################################
# TARGETS: PDF
###############################################################################
# create dynamic dev targets if opt is part of the goals
ifeq (dev,$(filter dev,$(MAKECMDGOALS)))
DEV := -dev
dev: ;
endif

# generic pdf function
define make_pdf =
	$(call echo_stage,$(1))
	$(call find_tool_or_exit,$(ADOCTORPDF))
	$(ADOCTORPDF) $(basename $(ADOCTOR_DSTFILE))$(DEV)$(suffix $(ADOCTOR_DSTFILE)) \
		-a pdf-theme=$(4) \
		$(ADOCTOR_PDF_FONTS) $(ADOCTOR_PDF_EXTENDERS) $(ADOCTOR_IMGDIR) \
		$(2) $(3) $(ADOC_PDF_EXTRA_OPTS) $<
endef

# target to generate pdf for screen
pdfs: pdf-screen pdf-prepress pdf-kdp

# target to generate pdf for screen
pdf-screen: $(PDF_SCREEN)

PDF_DEPS := $(BOOK_ADOC) $(IMGFILES) $(THEME_FILE) $(EXTENDERS_RB) $(FONTS)

# rule to generate pdf for screen
$(PDF_SCREEN): $(PDF_DEPS) $(COVERS) $(THEME_FILE_SCREEN)
	$(call make_pdf,pdf-screen,$(COVER_OPTS),$(ADOCPDF_SCREEN_OPTS),$(THEME_FILE_SCREEN))

# target to generate optimized pdf for screen
pdf-screen-opt: $(PDF_SCREEN_OPT)

# rule to generate optimized pdf for screen
$(PDF_SCREEN_OPT): $(PDF_SCREEN)
	$(call pdf_opt,Optimized PDF Screen)

# target to generate pdf for prepress (ready for printing)
pdf-prepress: $(PDF_PREPRESS)

# rule to generate pdf for prepress
$(PDF_PREPRESS): $(PDF_DEPS) $(COVERS)
	$(call make_pdf,pdf-prepress,$(COVER_OPTS),$(ADOCPDF_PREPRESS_OPTS),$(THEME_FILE))

# target to generate optimized pdf for prepress
pdf-prepress-opt: $(PDF_PREPRESS_OPT)

# rule to generate optimized pdf for prepress
$(PDF_PREPRESS_OPT): $(PDF_PREPRESS)
	$(call pdf_opt,Optimized PDF Prepress)

# target to generate pdf for kdp
pdf-kdp: $(PDF_KDP)

# rule to generate pdf for kdp - NO COVERS - uploaded separately
$(PDF_KDP): $(PDF_DEPS)
	$(call make_pdf,pdf-kdp,$(COVER_EMPTY_OPTS),$(ADOCPDF_PREPRESS_OPTS),$(THEME_FILE))

# target to generate optimized pdf for prepress
pdf-kdp-opt: $(PDF_KDP_OPT)

# rule to generate optimized pdf for prepress
$(PDF_KDP_OPT): $(PDF_KDP)
	$(call pdf_opt,Optimized PDF Kdp)

###############################################################################
# TARGETS: EPUB3
###############################################################################
# the images need to be in a sub-directory of where the file is generated for
# proper in-epub referencing. If the attribute imagedir is used with ../.., the
# files will be found during generation, but the link will be wrong (upper dir)
# in the resulting epub. Additionally, the covers need a different option
# to have them found hence the specific cover_front/back_epub option, because
# if not, the file will be sought where the book-adoc target is generated
# And because the output file is generated in place, the final target dir is
# not generated by asciidoctor which means it needs to be created, if not there
# and the file moved
# GOAL: do not copy images to all languages build dirs (linking is not possible
# under windows)
###############################################################################
# Generic function to make an epub
define make_epub =
	$(call echo_stage,$(1))
	$(call find_tool_or_exit,$(ADOCTOREPUB))
	$(ADOCTOREPUB) $(ADOCTOR_DSTFILE_CURDIR) $(2) $<
	$(call makedir_for_file_and_move,$@)
	-rm -rf $(EPUB_TMP)  # if there from previous run
	$(call makedir_dir,$(EPUB_TMP))
	for i in `ls $(THEME_EPUB)/`; do cp -r $(THEME_EPUB)/$$i $(EPUB_TMP); done
	cp -r $(FONTS_DIR) $(EPUB_TMP)
	$(ZIP) -r $@ $(EPUB_TMP)
	-rm -rf $(EPUB_TMP)
endef

EPUB_DEPS := $(IMGFILES) $(EPUB_STYLES) $(FONTS)

# target to generate epub
epub: $(EPUB)

# rule to generate epub
$(EPUB): $(BOOK_ADOC) $(COVERS_EPUB) $(EPUB_DEPS)
	$(call make_epub,epub3,$(COVER_EPUB_OPTS))

# target to generate epub for kdp - no front cover (the backcover can be there)
epub-kdp: $(EPUB_KDP)

# target to generate tiff for ebook
tiff: $(COVER_EPUB_FRONT_TIFF)

# rule to generate epub for kdp, i.e.: epub without the cover
$(COVER_EPUB_FRONT_TIFF): $(COVER_EPUB_FRONT)
	$(call find_tool_or_exit,$(CONVERT))
	$(CONVERT) $< $@

$(EPUB_KDP): $(BOOK_ADOC_EPUB) $(COVERS_EPUB_KDP) $(EPUB_DEPS)
	$(call make_epub,epub3-kdp,)

###############################################################################
# TARGETS: EPUB2
###############################################################################
# Some shops like epubli will only take epub2 and asciidoctor only generates
# epub3. This requires switching to pandoc which generates (with escapes in the
# the text if needed) epub2 compliant output.
#
# Note: ADOC_HEADER is part of BOOK_MD and therefore already a dependency
###############################################################################
# Generic function to make epub2
define make_epub2 =
	$(call echo_stage,$(1))
	$(call find_tool_or_exit,$(PANDOC))
	$(call makedir_for_file, $@)
	$(PANDOC) $(PD_MD_2_EPUB2) $(PD_TOP_LVL_DIV) $(PD_DEFAULTS) $(2) -o $@ $<
	$(BOOKFIX) $(BF_FIX_EPUB_DATE) -o $@ $(ADOC_HEADER)
endef

$(COVER_EPUB2_FRONT): $(COVER_EPUB_FRONT)
	$(call find_tool_or_exit,$(CONVERT))
	$(CONVERT) $< -resize $(COVER_EPUB2_WIDTH)x$(COVER_EPUB2_HEIGHT) $@

$(COVER_EPUB2_BACK): $(COVER_EPUB_BACK)
	$(call find_tool_or_exit,$(CONVERT))
	$(CONVERT) $< -resize $(COVER_EPUB2_WIDTH)x$(COVER_EPUB2_HEIGHT) $@

# target to generate epub2
epub2: $(EPUB2)

# rule to generate epub2
# both covers are a dependency, because the backcover is inside the content
$(EPUB2): $(BOOK_MD_EPUB2) $(DEFAULTS_MD) $(IMGFILES) $(COVERS_EPUB2)
	$(call make_epub2,epub2,$(PD_COV_FRONT_EPUB2))

# rule to generate epub2-kdp, i.e.: epub2 without the cover
epub2-kdp: $(EPUB2_KDP)

# rule to generate epub2
# back cover is a dependency, because it is inside the content
$(EPUB2_KDP): $(BOOK_MD_EPUB2) $(DEFAULTS_MD) $(IMGFILES) $(COVER_EPUB2_BACK)
	$(call make_epub2,epub2-kdp,)

###############################################################################
# TARGETS: HTML/XML
###############################################################################
# Generates an html/docbook document which expects the images to be in a subdir
# besides the document
define make_xhtml_images =
	$(call echo_stage,$(1))
	$(call find_tool_or_exit,$(ADOCTOR))
	$(ADOCTOR) -b $(1) $(ADOCTOR_DSTFILE_CURDIR) $<
	$(call makedir_for_file_and_move,$@)
endef

# Generates an html/docbook document which expects the images to be in the root
# directory of the build system. For test purposes
define make_xhtml =
	$(call echo_stage,$(1))
	$(call find_tool_or_exit,$(ADOCTOR))
	$(ADOCTOR) -b $(1) $(ADOCTOR_DSTFILE) $(2) $<
endef

# target to generate html
html: $(HTML)

# images not a dependency because are not in doc, put in directory besides the
# output if images are desired
# rule to generate html - no covers - has no function
$(HTML): $(BOOK_ADOC)
	$(call make_xhtml,html5)

# target to generate zip with html and images
html-zip: $(HTML_ZIP)

# rule which creates zip storing file with no path and images with
# so that everything matches
$(HTML_ZIP): $(HTML)
	-rm -f $@
	$(ZIP) --junk-paths $@ $<
	$(ZIP) $@ `grep -oP '(?<=<img src=")($(IMG_DIR)[^"]+)' $<`

# target to generate html for in-place reading
html-test: $(HTML_TEST)

# has relative links to source images
$(HTML_TEST): $(BOOK_ADOC)
	$(call make_xhtml,html5,$(ADOCTOR_IMGDIR))

# target to generate docbook
docbook: $(DOCBOOK)

# rule to generate docbook
$(DOCBOOK): $(BOOK_ADOC) $(IMGFILES)
	$(call make_xhtml,docbook5)

###############################################################################
# TOUCH to force a rebuidl
###############################################################################
ifeq (touch,$(filter touch,$(MAKECMDGOALS)))
$(shell touch $(ADOC_HEADER))
touch: ;
endif

###############################################################################
# GITHUB Release
###############################################################################
gh-release: pdf-screen-opt pdf-prepress-opt
	$(call find_tool_or_exit,$(GHCLI))
	$(eval tmpdir := $(dir $(shell mktemp -u)))
	$(eval tmp_screen := $(tmpdir)/$(notdir $(PDF_SCREEN)))
	$(eval tmp_prepress := $(tmpdir)/$(notdir $(PDF_PREPRESS)))
	cp $(PDF_SCREEN_OPT) $(tmp_screen)
	cp $(PDF_PREPRESS_OPT) $(tmp_prepress)
	$(GHCLI) release create v$(BVERSION) $(tmp_screen) $(tmp_prepress)
	-rm -f $(tmp_screen) $(tmp_prepress)

###############################################################################
# CLEANING
###############################################################################
# delete files
clean:
	-rm -rf $(BASE_BUILD_DIR)

clean-books:
	-rm -rf $(BUILD_DIR_BOOK)/*

clean-adoc:
	-rm -rf $(BUILD_DIR_ADOC)/*

clean-md:
	-rm -rf $(BUILD_DIR_MD)/*

# cover pdf, epub, html
clean-%:
	-rm -rf $(BUILD_DIR_BOOK)/*.$*

clean-docbook:
	-rm -rf $(BUILD_DIR_BOOK)/*.xml

clean-mkdocs:
	-rm -rf $(BUILD_DIR_MKDOCS)/*

###############################################################################
# UTILITIES
###############################################################################
# renumber chapters
renumber:
	$(call find_tool_or_exit,$(PYTHON3))
	./bin/renumberer.py

# count the words/lines/chars of the chapters
count-%:
	@wc --$* $(SRCFILES)

# list source files
sources:
	@echo $(SRCFILES) | tr ' ' '\n'

sources-md:
	@echo $(ADOC_HEADERS) $(SRCFILES_MD) | tr ' ' '\n'

# show pdf files
show: show-pdf

show-pdf: show-pdf-screen

show-pdf-screen: $(PDF_SCREEN)
	$(call find_tool_or_exit,$(PDF_EVINCE))
	$(PATH_EVINCE) $< &

show-pdf-prepress: $(PDF_PREPRESS)
	$(call find_tool_or_exit,$(PDF_EVINCE))
	$(PATH_EVINCE) $< &

show-pdf-kdp: $(PDF_KDP)
	$(call find_tool_or_exit,$(PDF_EVINCE))
	$(PATH_EVINCE) $< &

kshow:
	$(eval evince-ps := $(shell ps -ax | grep $(PDF_EVINCE) 2>/dev/null))
	$(shell kill $(firstword $(evince-ps)) 2>/dev/null)

rshow: kshow show

# check if tools can be found
toolcheck:
	@echo The following commands/tools are considered to be always available
	@echo "  - awk, cp, cut, echo, exit, grep, head, mkdir, mktemp, mv, read"
	@echo "  - sed, sort, tail, test ([]), tr, which"
	@echo ----------------------------------------------------------------------
	@echo Checking for "$(KRAMDOC) for markdown to asciidoc conversion"
	$(call find_tool,$(KRAMDOC))
	@echo Checking for "$(ADOCTOR) for asciidoc parsing"
	$(call find_tool,$(ADOCTOR))
	@echo Checking for "$(ADOCTORPDF) for PDF generation"
	$(call find_tool,$(ADOCTORPDF))
	@echo Checking for "$(HEXAPDF) for PDF Size optimization"
	$(call find_tool,$(HEXAPDF))
	@echo Checking for "$(ADOCTOREPUB) for EPUB3 generations"
	$(call find_tool,$(ADOCTOREPUB))
	@echo Checking for "$(CONVERT) for auto-generation of KDP .tiff cover"
	$(call find_tool,$(CONVERT))
	@echo Checking for "$(ZIP) for packing operations on HTML/EPUB"
	$(call find_tool,$(ZIP))
	@echo Checking for "$(PYTHON3) for processing scripts"
	$(call find_tool,$(PYTHON3))
	@echo Checking for "$(PDF_EVINCE) for PDF viewing"
	$(call find_tool,$(PDF_EVINCE))
	@echo Checking for "$(PANDOC) for EPUB2 generation (must be improved)"
	$(call find_tool,$(PANDOC))
	@echo Checking for "$(PDM) for python env/package management for mkdocs"
	$(call find_tool,$(PDM))
	@echo Checking for "$(MKDOCS) for site generation (without pdm)"
	$(call find_tool,$(MKDOCS))
	@echo Checking for "$(MKDOCS) for site generation (WITH pdm if available)"
	$(call find_tool,$(MKDOCS),$(PDM_RUN))
	@echo Checking for "$(PKILL) for killing processes (procps package)"
	$(call find_tool,$(PKILL))
	@echo Checking for "$(GHCLI) to manage GitHub releases"
	$(call find_tool,$(GHCLI))

###############################################################################
# HELP
###############################################################################
help:
	@echo
	@echo "Language support"
	@echo "  Add blang=xx to make the target for a language (default 'en')"
	@echo "  example: 'pdf-screen blang=de' will make the target for german"
	@echo
	@echo "  Default/Requested Language: $(BLANG)"
	@echo "  Available Languages: $(BLANGS)"
	@echo "  Disabled Languages: $(XBLANGS)"
	@echo
	@echo "Publishing Targets"
	@echo "  pdf - (alias for pdf-screen)"
	@echo "  pdfs - (pdf-screen / pdf-prepress / pdf-kdp)"
	@echo "  pdf-screen / pdf-prepress / pdf-kdp (prepress / no covers)"
	@echo "    - Optimized pdf versions"
	@echo "      pdf-screen-opt / pdf-prepress-opt / pdf-kdp-opt"
	@echo "  opt (Enables optimized targets. Example: 'opt pdf-kdp')"
	@echo "  epub / epub-kdp (back cover only)"
	@echo "  epub2 / epub2-kdp (back cover only)"
	@echo "  html / html-zip / html-test (in-place reading) / docbook"
	@echo "    (no covers in html/docbook)"
	@echo "  mkdocs (Publish for the mkdocs subdirectory configuration)"
	@echo "  mkdocs-pdf (Publish mkdocs and add PDF Download Page)"
	@echo "  mkdocs-pdfopt (Publish mkdocs and add PDF Download Page with optimized PDFs)"
	@echo "  mkdocs-fakepdf (Publish mkdocs and add PDF Download Page with no PDFs)"
	@echo "  mkdocs-serve (Publish and start serving locally)"
	@echo "  mkdocs-bserve (Publish and start serving locally in the background)"
	@echo "  mkdocs-kill (kill a mkdocs running instance)"
	@echo "  mkdocs-build (Build the mkdocs site docs)"
	@echo "  mkdocs-deploy (Deploy to GitHub Pages)"
	@echo "     (also mk-xxx as shorthand for all actions)"
	@echo "  gh-release (Make a Release on the GitHub Repo)"
	@echo
	@echo "Selected build of specific chapters"
	@echo "  - xxxx yyyy do not need to be full filenames"
	@echo "  buildchap=xxxx or buildchap='xxxx yyyy'"
	@echo
	@echo "Auxiliary Targets"
	@echo "  touch (ensure rebuild even if intermediates are unchanged)"
	@echo "  dev (add suffix to final target to separate from final versions)"
	@echo "  isbn (Add isbn to final target name and use back cover with isbn)"
	@echo
	@echo "Intermediate Targets"
	@echo "  book-adoc / book-adoc-epub (asciidoc book)"
	@echo "  book-md / book-md-epub2 (markdown book)"
	@echo
	@echo "All Targets"
	@echo "  all - make all targets (default language or BLANG=$(BLANG))"
	@echo "  all-xx - make all targets for lang xx (if available)"
	@echo "  all-langs - make all for all languages ($(BLANGS))"
	@echo
	@echo "Cleaning Targets"
	@echo "  clean - remove everything in all languages"
	@echo "  clean-adoc - remove only asciidoc book sources"
	@echo "  clean-md - remove only md book sources"
	@echo "  clean-books - remove all final book formats"
	@echo "  clean-pdf - remove only pdf targets"
	@echo "  clean-epub - remove only epub targets"
	@echo "  clean-html - remove only html targets"
	@echo "  clean-docbook - remove only docbook targets"
	@echo "  clean-mkdocs - remove only mkdocs targets"
	@echo
	@echo "Tools"
	@echo "  toolcheck - check if needed tooling is found "
	@echo
	@echo "Renumber chapters to align with chapter numbers"
	@echo "  renumber"
	@echo
	@echo "Count Statistics"
	@echo "  count-chars / count-words / count-lines "
	@echo
	@echo "List Sources for targets"
	@echo "  sources - (for asciidoc target)"
	@echo "  sources-md - (for md target for pandoc for epub2)"
	@echo
	@echo "Show PDF Books"
	@echo "  show / show-pdf (alias for show-pdf-screen)"
	@echo "  show-pdf-screen / show-pdf-prepress / show-pdf-kdp"
	@echo "  kshow (kill program showing PDF)"
	@echo "  rshow (kill and show)"
	@echo
	@echo " ---- EXTRA PROJECT OPTIONS ----"
	@echo "Project Option Group"
	@echo "  Add option=xx to achieve something"
	@echo "  Valid values: per project"
	@echo "  Default/Requested with 'varname': $(varname)"
