SOAP v2.7

The Qt SOAP project provides basic web service support with
version 1.1 of the SOAP protocol.



Notes:

This is a partial implementation of SOAP v1.1.

- Server side SOAP is not supported. - References to values (id &
href attributes) are not supported. - Only arrays with less than 5
dimensions are supported. - Namespaces for types are not checked.
Only the type names are used. - The encodingStyle attribute is
ignored. The serialization and encoding rules from section 5 in
the SOAP v1.1 specification are assumed regardless of the value of
the encodingStyle attribute. - QtSoapType does not have accessors
for attributes, which means for example that actor, mustUnderstand
and so on are not accessible in headers. - The SOAP root attribute
is not supported.

