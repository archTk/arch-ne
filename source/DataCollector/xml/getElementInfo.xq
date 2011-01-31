declare variable $inputDocument external;
declare variable $rootName external;
declare variable $rootParent external;
declare variable $xsdFile := doc($inputDocument);

let $rootType := data($xsdFile//xs:complexType[@name = $rootParent]//xs:element[@name = $rootName]/@type)
let $rootElement := $xsdFile//xs:complexType[@name = $rootType] 
let $subSeqEls := for $x in $rootElement//xs:sequence//xs:element
                return concat(string(data($x/@name)),string(","),string(data($x/@minOccurs)),string(","),string(data($x/@maxOccurs)))
let $subAllEls := for $x in $rootElement//xs:all//xs:element
                return concat(string(data($x/@name)),string(","),string(data($x/@minOccurs)),string(","),string(data($x/@maxOccurs)))
let $subChoiceEls := for $x in $rootElement//xs:choice//xs:element
                return concat(string(data($x/@name)),string(","),string(data($x/@minOccurs)),string(","),string(data($x/@maxOccurs)))
let $subGroup := let $y := $xsdFile//xs:attributeGroup[@name = data($rootElement//xs:attributeGroup/@ref)]
                    for $z in $y//xs:attribute
                          return concat(string($z/@name),string("="),string($z/@type))

let $subAtts := for $y in $rootElement//xs:attribute
                             return concat(string($y/@name),string("="),string($y/@type))
let $allMembers := for $x in ((string($rootType),"#",$subGroup , $subAtts, "#", $subSeqEls,$subAllEls,"#",$subChoiceEls)) return $x
return $allMembers

