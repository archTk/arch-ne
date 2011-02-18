declare variable $inputDocument external;
declare variable $rootName external ;
declare variable $rootParent external ;

declare function local:getSubElements($parent,$rootFile)
{
let $atts := local:getSubAttributes($parent,$rootFile)
let $attg := local:getAttributeGroups($parent,$rootFile)
for $x in $parent//xs:element
   let $y := $rootFile//xs:complexType[@name = data($x/@type)]
   let $z := $rootFile//xs:simpleType[@name = data($x/@type)]
   let $w := $rootFile//xs:simpleType[@name = data($z//xs:restriction/@base)]
   return  $atts | $attg | $z | $y | $w | local:getSubElements($y,$rootFile)
};

declare function local:getAttributeGroups($parent,$rootFile)
{
for $x in $parent//xs:attributeGroup
   let $y := $rootFile//xs:attributeGroup[@name = data($x/@ref)]
   return $y | local:getSubAttributes($y,$rootFile)
};

declare function local:getSubAttributes($parent,$rootFile)
{
for $x in $parent//xs:attribute
   let $y := $rootFile//xs:simpleType[@name = data($x/@type)]
   return $y 
};

let $xsdFile := doc($inputDocument)
let $rootType := data($xsdFile//xs:complexType[@name = $rootParent]//xs:element[@name = $rootName]/@type)
let $rootElement := $xsdFile//xs:complexType[@name = $rootType] 
let $subElements := local:getSubElements($rootElement,$xsdFile)
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



