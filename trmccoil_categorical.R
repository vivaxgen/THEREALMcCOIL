#!/usr/bin/env Rscript

library(optparse)

option.list <- list(
    make_option(c("-o", "--outfile"),
                help="output filename"),
    make_option(c("-l", "--libpath"),
                help="library path to THEREALMcCOIL.")
)

parser <- OptionParser(usage="%prog [options] inputfile",
                       option_list=option.list)
args <- parse_args(parser, positional_arguments=1)

# infile in args$args

# set path to THEREALMcCOIL categorical path

libpath = paste(args$options$libpath, "categorical_method", sep='/')
source_file = paste(libpath, "McCOIL_categorical.R", sep='/')

#infile <- normalizePath(args$args)
#outfile <- normalizePath(args$options$outfile, mustWork=F)

working_path=getwd()

source(source_file)

infile = args$args
outfile = args$options$outfile

warning("Reading from ", infile, "and writing to ", outfile)

data0=read.table(infile, head=T)
data=data0[,-1]
rownames(data)=data0[,1]
McCOIL_categorical(data,maxCOI=25, threshold_ind=20, threshold_site=20, totalrun=1000, burnin=100,
                   M0=15, e1=0.05, e2=0.05, err_method=3,
                   path=libpath, output=outfile)
