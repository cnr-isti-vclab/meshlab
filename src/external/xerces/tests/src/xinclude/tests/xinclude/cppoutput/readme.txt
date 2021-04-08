The following changes are made to the java xinclude test output files;

1) [ standalone="no" ] is added to the XML Declaration in each file.

2) Attributes on the included top level include element are reordered
alphabetically as this is the xerces c++ behaviour!

3) Removed a space from after the xml declaration and before the first
document element (thats a single space btw).

4) xerces c++ parser's don't expand quotation entities to &quot. Java does. Investigate!

5) added xmlns:xml="http://www.w3.org/XML/1998/namespace" attribute to appropriate top level elements

