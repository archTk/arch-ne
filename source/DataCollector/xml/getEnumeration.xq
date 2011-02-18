declare variable $inputDocument external;
declare variable $attributeType external;

for $x in doc($inputDocument)//xs:simpleType[@name = $attributeType]//xs:enumeration
       return string(data($x/@value))

