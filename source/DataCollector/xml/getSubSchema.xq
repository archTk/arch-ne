declare variable $inputDocument external;
declare variable $rootName external ;
declare variable $rootType external ;
declare variable $xsdFile := doc($inputDocument) ;

declare function local:getSubElements($parent)
{
let $atts := local:getSubAttributes($parent)
let $attg := local:getAttributeGroups($parent)
for $x in $parent//xs:element
   let $y := $xsdFile//xs:complexType[@name = data($x/@type)]
   let $z := $xsdFile//xs:simpleType[@name = data($x/@type)]
   let $w := $xsdFile//xs:simpleType[@name = data($z//xs:restriction/@base)]
   return  $atts | $attg | $z | $y | $w | local:getSubAttributes($y) | local:getSubElements($y)
};

declare function local:getAttributeGroups($parent)
{
for $x in $parent//xs:attributeGroup
   let $y := $xsdFile//xs:attributeGroup[@name = data($x/@ref)]
   return $y | local:getSubAttributes($y)
};

declare function local:getSubAttributes($parent)
{
for $x in $parent//xs:attribute
   let $y := $xsdFile//xs:simpleType[@name = data($x/@type)]
   return $y 
};

let $rootElement := $xsdFile//xs:complexType[@name = $rootType] 
let $subElements := local:getSubElements($rootElement)
let $subNames := for $k in $subElements
                                 return data($k/@name)
let $uniqueNames := distinct-values($subNames)
let $finalElements := for $name in $uniqueNames
                                      let $a := $xsdFile//xs:simpleType[@name = $name]
                                      let $b := $xsdFile//xs:complexType[@name = $name]
                                      let $c := $xsdFile//xs:attributeGroup[@name = $name]
                                      return $a | $b | $c

return 
<xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
<xsd:element name="{$rootName}" type="{$rootType}" />
{$rootElement}
{$finalElements}
</xsd:schema>

