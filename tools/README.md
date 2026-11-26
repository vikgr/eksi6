# Tools

`assembly-guide.html` is the source of `docs/pdf/eksi6-first-test-assembly-guide.pdf`.

Rebuild the PDF (from this folder):

```
google-chrome --headless=new --no-pdf-header-footer \
  --print-to-pdf=../docs/pdf/eksi6-first-test-assembly-guide.pdf \
  file://$PWD/assembly-guide.html
```

`svgstyle.txt` holds the shared style block used by the SVG diagrams in `docs/diagrams/`.
