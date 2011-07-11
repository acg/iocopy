#!/usr/bin/env python

import os, sys
from os.path import splitext, basename
import markdown
import jinja2

slides = []
slidefiles = open('slides/index.txt').read().split('\n')

for f in slidefiles:
  if f == '': continue
  name = splitext(basename(f))[0]
  text = open('slides/%s' % f).read()
  html = markdown.markdown( text )
  slides.append( { 'name': name, 'content': html } )

data = {
  'page': { 'title': open('data/title.txt').read() },
  'slides': slides,
}

template_string = open(sys.argv[1]).read()
template = jinja2.Template( template_string )
print template.render( data )

