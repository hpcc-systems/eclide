#pragma once

#include "ecllib.h"

using namespace boost::spirit::classic;

struct ecl_grammar : public grammar<ecl_grammar>
{
    enum 
    {
        unknownID, 
        programID,
        queryID,
        commentID,
        blockCommentID,
        lineCommentID,
        GrETLCommentID,
        GrETLContentID,
        invokeCommentID,
        invokeID,
        invokeLabelID,
        commentContentID,
        importID,
        scopeFlagID,
        selfFlagID,
        moduleID,
        moduleParamsID,
        moduleExportID,
        otherModuleID,
        macroID,
        macro_noparamID,
        macro_emptyparamID,
        macro_paramID,
        macroLabelID,
        macroParamsID,
        macroContentID,
        boolLiteralID,
        emptySetID,
        identifierID,
        identifierReferenceID,
        qualifiedIdentifierID,
        declAndAssignID,
        declarationID,
        assignmentID,
        recordID,
        recordBodyID,
        projectID,
        datasetID,
        datasetFormatID,
        datasetFormatFlatID,
        datasetFormatCsvID,
        datasetFormatCsvAttrID,
        datasetFormatCsvAttrHeadingID,
        datasetFormatCsvAttrMaxlengthID,
        datasetFormatCsvAttrSeparatorID,
        datasetFormatCsvAttrTerminatorID,
        datasetFormatCsvAttrValueID,
        datasetFormatXmlID,
        datasetFormatXmlAttrID,
        serviceID,
        serviceDeclarationID,
        paramaterizedAttributeID,
        sizeOfID,
        transformID,
        castID,
        typeID,
        typeBooleanID,
        typeIntegerID, 
        typeRealID,
        typeDecimalID,
        typeUDecimalID,
        typeStringID,
        typeQStringID,
        typeUnicodeID,
        typeDataID,
        typeVarStringID,
        typeVarUnicodeID,
        typeUnsignedID,
        constParamID,
        localeID,
        otherID,
        otherSemiID,
        otherEolID,
        attributeListID,
        otherEofID,
        parameterID,
        parameterListID,
        stringLiteralID,
        numericLiteralID,
        valueID,
        valueOrDefaultID,
        valueListID,
        functionDeclarationID,
        selfIdentifierID,
        outputID,
    };
    
    template <typename ScannerT>
    struct definition
    {
        definition(ecl_grammar const& /*self*/)
        {

            /*
                *a Kleene star Match a zero (0) or more times 
                +a Positive Match a one (1) or more times 
                !a Optional Match a zero (0) or one (1) time 
                a % b List Match a list of one or more repetitions of a separated by occurrences of b. This is the same as a >> *(b >> a). Note that a must not also match b 
            */
        
        /*--INVOKE(ccc)--
        'ccc:  %cluster%.%module%.%attribute%'
        */
 
        /*--GrETL--
        <xml/>
        */

            //-----------------------------------------------------------------
            // KEYWORDS
            //-----------------------------------------------------------------
            keywords =
                "import", "const", "export", "shared", "output", "count", "record", "project", "dataset", 
                "transform", "end", "module", "macro", "endmacro", "self", "service", 
                "false", "true", "sizeof", "flat";
        
            //-----------------------------------------------------------------
            // OPERATORS
            //-----------------------------------------------------------------

            strlit<>    EQUIV(":=");    
            chlit<>     COMMA(',');
            chlit<>     LPAREN('(');
            chlit<>     RPAREN(')'); 
            chlit<>     LSQUAREPAREN('[');
            chlit<>     RSQUAREPAREN(']'); 
            chlit<>     LBRACE('{');
            chlit<>     RBRACE('}'); 
            chlit<>     SEMI(';');
            chlit<>     COLON(':');
            chlit<>     LT('<');
            chlit<>     GT('>');
            strlit<>    LE("<=");
            strlit<>    GE(">=");
            chlit<>     EQUAL('=');
            strlit<>    NE("!=");    

            chlit<>     PLUS('+');
            chlit<>     MINUS('-');
            chlit<>     STAR('*');
            chlit<>     DOT('.');        

            //-----------------------------------------------------------------
            // SPECIAL
            //-----------------------------------------------------------------

            strlit<>    BCOMMENT_START("/*");
            strlit<>    BCOMMENT_END("*/");
            strlit<>    INVOKE_START("--INVOKE(");
            strlit<>    INVOKE_END(")--");
            strlit<>    GRETL_MARKER("--GrETL--");

            //---------------------------------------
            // TOKENS
            //---------------------------------------
            typedef inhibit_case<strlit<> > token_t;

            token_t EXPORT      = as_lower_d["export"];
            token_t SHARED      = as_lower_d["shared"];
            token_t OUTPUT      = as_lower_d["output"];
            token_t COUNT       = as_lower_d["count"];
            token_t RECORD      = as_lower_d["record"];
            token_t PROJECT     = as_lower_d["project"];
            token_t LEFT		= as_lower_d["left"];
            token_t DATASET     = as_lower_d["dataset"];
            token_t IMPORT		= as_lower_d["import"];
            token_t SERVICE		= as_lower_d["service"];
            token_t SIZEOF		= as_lower_d["sizeof"];
            token_t TRANSFORM   = as_lower_d["transform"];
            token_t SELF	    = as_lower_d["self"];
            token_t END         = as_lower_d["end"];
            token_t MODULE      = as_lower_d["module"];
            token_t MACRO       = as_lower_d["macro"];
            token_t ENDMACRO    = as_lower_d["endmacro"];

            token_t FLAT        = as_lower_d["flat"];
            token_t CSV			= as_lower_d["csv"];
            token_t XML			= as_lower_d["xml"];
            token_t HEADING		= as_lower_d["heading"];
            token_t SEPARATOR   = as_lower_d["separator"];
            token_t MAXLENGTH	= as_lower_d["maxlength"];
            token_t TERMINATOR  = as_lower_d["terminator"];

            token_t BIG_ENDIAN  = as_lower_d["big_endian"];
            token_t LITTLE_ENDIAN  = as_lower_d["little_endian"];
            token_t EBCDIC		= as_lower_d["ebcdic"];
            token_t ASCII		= as_lower_d["ascii"];
            
            token_t _CONST		= as_lower_d["const"];

            token_t BOOLEAN     = as_lower_d["boolean"];
            token_t UNSIGNED    = as_lower_d["unsigned"];
            token_t INTEGER     = as_lower_d["integer"];
            token_t DECIMAL     = as_lower_d["decimal"];
            token_t UDECIMAL    = as_lower_d["udecimal"];
            token_t STRING      = as_lower_d["string"];
            token_t QSTRING     = as_lower_d["qstring"];
            token_t ECL_UNICODE = as_lower_d["unicode"];
            token_t DATA        = as_lower_d["data"];
            token_t VARSTRING   = as_lower_d["varstring"];
            token_t VARUNICODE  = as_lower_d["varunicode"];
            token_t ECL_REAL    = as_lower_d["real"];

            token_t ECL_TRUE    = as_lower_d["true"];
            token_t ECL_FALSE   = as_lower_d["false"];

          //-----------------------------------------
          //	Start grammar definition
          //-----------------------------------------

            identifier =
                leaf_node_d 
                [
                    nocase_d
                    [
                        lexeme_d
                        [
                            ((alpha_p | '_') >> *(alnum_p | '_'))
                        ]
                    ]
                ]
                ;

            stringLiteral =
                leaf_node_d 
                [
                    lexeme_d
                    [
                        ch_p('\'') >>  
                        *( strlit<>("\\'") | anychar_p - ch_p('\'') ) 
                        >> ch_p('\'')
                    ]
                ]
                ;

            //-----------------------------------------------------------------
            // RULES
            //-----------------------------------------------------------------

            program 
                =	root_node_d[+(query >> !(discard_node_d[SEMI] | discard_node_d[end_p]))];
            query 
                =	import
                |	module
                |	macro
                |	transform
                |	service
                |	assignment
                |	declAndAssign
                |	paramaterizedAttribute
                |	GrETLComment
                |	invokeComment
                |	comment
                |	output
                |	other
                ;
            scopeFlag
                =	EXPORT | SHARED
                ;
            selfFlag
                =	SELF 
                >>	discard_node_d[DOT]
                ;
            comment 
                =	blockComment
                |	lineComment
                ;
            GrETLComment
                =	confix_p(discard_node_d[BCOMMENT_START], GrETLContent, discard_node_d[BCOMMENT_END])
                ;
            GrETLContent
                =	discard_node_d[GRETL_MARKER] >> commentContent
                ;
            invokeComment 
                =	confix_p(discard_node_d[BCOMMENT_START], invoke, discard_node_d[BCOMMENT_END])
                ;
            invoke
                =	invokeLabel >> commentContent
                ;
            invokeLabel
                =	INVOKE_START >> lexeme_d[leaf_node_d[*(anychar_p - INVOKE_END)]] >> INVOKE_END
                ;
            lineComment 
                =	leaf_node_d[comment_p("//")]
                ;
            blockComment 
                =	confix_p(discard_node_d[BCOMMENT_START], commentContent, discard_node_d[BCOMMENT_END])
                ;
            commentContent
                =	lexeme_d[leaf_node_d[*(anychar_p - BCOMMENT_END)]]
                ;
            import
                =	IMPORT
                >>	identifier 
                >> *(COMMA >> identifier)
                ;
            module
                =  scopeFlag
                >> functionDeclaration
                >> discard_node_d[EQUIV]
                >> MODULE 
                >> *(moduleExport | otherModule)
                >> END
                ;

            moduleParams
                =	infix_node_d[type >> identifier >> *(discard_node_d[COMMA] >> type >> identifier)]
                ;
            moduleExport
                = EXPORT 
                >> type 
                >> identifier 
                >> discard_node_d[EQUIV] 
                >> otherSemi
                ;
            otherModule
                = lexeme_d[leaf_node_d[+(anychar_p - EXPORT - END)]]
                ;
            macro
                =	macro_noparam
                |	macro_emptyparam
                |	macro_param
                ;
            macro_noparam
                =	!scopeFlag
                >>	identifier
                >>	discard_node_d[EQUIV]
                >>	macroContent
                ;
            macro_emptyparam
                =	!scopeFlag
                >>	identifier
                >>  inner_node_d[LPAREN >> RPAREN]
                >>	discard_node_d[EQUIV]
                >>	macroContent
                ;
            macro_param
                =	!scopeFlag
                >>	identifier
                >>  inner_node_d[LPAREN >> macroParams >> RPAREN]
                >>	discard_node_d[EQUIV]
                >>	macroContent
                ;
            macroLabel
                =	identifier
                ;
            macroParams
                =	infix_node_d[identifier >> *(COMMA >> identifier)]
                ;
            macroContent
                =	inner_node_d[MACRO >> lexeme_d[leaf_node_d[*(anychar_p - ENDMACRO)]] >> ENDMACRO]
                ;
            boolLiteral
                = ECL_FALSE
                | ECL_TRUE
                ;
            emptySet
                = discard_node_d[LSQUAREPAREN] >> discard_node_d[RSQUAREPAREN]
                ;
            identifierReference
                = (qualifiedIdentifier | identifier)
                //>> !parameterList
                ;
            qualifiedIdentifier
                = identifier
                >> +(discard_node_d[DOT] >> identifier)
                ;
            selfIdentifier
                = discard_node_d[SELF]
                >> discard_node_d[DOT]
                >> identifier
                ;
            declaration 
                =	type
                >>	identifier
                >>  !attributeList
                ;
            assignment 
                =	!scopeFlag
                >>  !selfFlag
                >>	identifier
                >>	discard_node_d[EQUIV]
                >>	(dataset | record | project | value | other)
                ;
            serviceDeclaration 
                =	!type
                >>	identifier
                >>	parameterList
                >>	discard_node_d[COLON]
                >>	otherSemi //serviceDescription
                ;
            paramaterizedAttribute
                =	identifierReference
                >>  valueList
                ;
            sizeOf
                =	discard_node_d[SIZEOF]
                >>  discard_node_d[LPAREN]
                >>	identifierReference
                >>  discard_node_d[RPAREN]
                ;
            declAndAssign 
                =	type
                >>	assignment 
                ;
            output
                =	discard_node_d[OUTPUT]
                >>	discard_node_d[LPAREN]
                >>	(identifier)
                >> !(discard_node_d[COMMA] >> record)
                >>  discard_node_d[RPAREN]
                ;
            record 
                =	discard_node_d[RECORD]
                >>  *(recordBody >> !discard_node_d[SEMI])
                >>  discard_node_d[END]
                ;
            recordBody
                =	declAndAssign 
                |	assignment 
                |	declaration 
                |	comment
                ;
            project
                =	discard_node_d[PROJECT] 
                >>  discard_node_d[LPAREN]
                >>  !(qualifiedIdentifier | identifier) 
                >>  COMMA
                >>  !(identifier)
                >>  discard_node_d[LPAREN]
                >>  discard_node_d[LEFT]
                >>  discard_node_d[RPAREN]
                >>  discard_node_d[RPAREN]
                ;
            dataset
                =	discard_node_d[DATASET] 
                >>  discard_node_d[LPAREN]
                >>  *(valueOrDefault)
                >>  datasetFormat
                >>  discard_node_d[RPAREN]
                ;
            datasetFormat
                =	datasetFormatFlat | datasetFormatCsv | datasetFormatXml
                ;
            datasetFormatFlat
                =	discard_node_d[FLAT]
                ;
            datasetFormatCsv
                =	discard_node_d[CSV]
                >>	discard_node_d[LPAREN]
                >>	datasetFormatCsvAttr % discard_node_d[COMMA]
                >>  discard_node_d[RPAREN]
                ;
            datasetFormatCsvAttr
                =	datasetFormatCsvAttrHeading | datasetFormatCsvAttrMaxlength | datasetFormatCsvAttrSeparator | datasetFormatCsvAttrTerminator
                ;
            datasetFormatCsvAttrHeading
                =	discard_node_d[HEADING]
                >>	datasetFormatCsvAttrValue
                ;
            datasetFormatCsvAttrMaxlength
                =	discard_node_d[MAXLENGTH]
                >>	datasetFormatCsvAttrValue
                ;
            datasetFormatCsvAttrSeparator
                =	discard_node_d[SEPARATOR]
                >>	datasetFormatCsvAttrValue
                ;
            datasetFormatCsvAttrTerminator
                =	discard_node_d[TERMINATOR]
                >>	datasetFormatCsvAttrValue
                ;
            datasetFormatCsvAttrValue
                =	discard_node_d[LPAREN] 
                >>	lexeme_d[leaf_node_d[*(anychar_p - RPAREN)]] 
                >>	discard_node_d[RPAREN]
                ;
            datasetFormatXml
                =	discard_node_d[XML]
                >>	discard_node_d[LPAREN]
                >>	datasetFormatXmlAttr % discard_node_d[COMMA]
                >>  discard_node_d[RPAREN]
                ;
            datasetFormatXmlAttr
                =	stringLiteral
                ;
            service 
                =	!scopeFlag
                >>	identifier
                >>	discard_node_d[EQUIV]
                >>	SERVICE 
                >>	*(serviceDeclaration | comment)
                >>	END
                ;
            transform 
                =  identifierReference
                >> identifier
                >> discard_node_d[LPAREN]
                >> !(identifierReference >> identifier)
                >> discard_node_d[RPAREN]
                >> discard_node_d[EQUIV]
                >> discard_node_d[TRANSFORM]
                >> *(recordBody >> !discard_node_d[SEMI]) 
                >> discard_node_d[END]
                ;
            parameter
                =  !constParam
                >> (type | identifierReference) >> identifier 
                ;
            parameterList
                = discard_node_d[LPAREN]
                >> !parameter % discard_node_d[COMMA]
                >> discard_node_d[RPAREN]
                ;
            functionDeclaration
                =  identifier
                >> parameterList
                ;
            numericLiteral
                = int_p
                | uint_p
                //| hex_p
                ;
            value
                = !cast
                >> (boolLiteral | stringLiteral | numericLiteral | sizeOf | paramaterizedAttribute | identifierReference | emptySet)
                ;
            cast
                = discard_node_d[LPAREN]
                >> type
                >> discard_node_d[RPAREN]
                ;
            valueOrDefault
                = !value
                >> *blockComment
                >> discard_node_d[COMMA]
                ;
            valueList
                = discard_node_d[LPAREN]
                >> *(valueOrDefault)
                >> !value
                >> discard_node_d[RPAREN]
                ;
            constParam
                =  !_CONST
                ;
            type
                =  typeBoolean
                |	typeInteger 
                |	typeReal
                |	typeDecimal
                |	typeUDecimal
                |	typeString
                |	typeQString
                |	typeUnicode
                |	typeData
                |	typeVarString
                |	typeVarUnicode
                |	typeUnsigned
                ;

            typeBoolean
                =	BOOLEAN
                ;
            typeInteger 
                =	!(BIG_ENDIAN | LITTLE_ENDIAN) >> !UNSIGNED >> lexeme_d[INTEGER >> !range_p('1', '8')]
                ;
            typeReal
                =	lexeme_d[ECL_REAL >> !(ch_p('4') | ch_p('8'))]
                ;
            typeDecimal
                =	!UNSIGNED >> lexeme_d[DECIMAL >> uint_p >> !(discard_node_d[ch_p('_')] >> uint_p)]
                ;
            typeUDecimal
                =	lexeme_d[UDECIMAL >> !(uint_p >> !(discard_node_d[ch_p('_')] >> uint_p))]
                ;
            typeString
                =	!(EBCDIC | ASCII) >> lexeme_d[STRING >> !uint_p]
                ;
            typeQString
                =	lexeme_d[QSTRING >> !uint_p]
                ;
            typeUnicode
                =	lexeme_d[ECL_UNICODE >> !(alpha_p >> alpha_p) >> !uint_p]
                ;
            typeData
                =	lexeme_d[DATA >> !uint_p]
                ;
            typeVarString
                =	lexeme_d[VARSTRING >> !uint_p]
                ;
            typeVarUnicode
                =	lexeme_d[VARUNICODE >> !(alpha_p >> alpha_p) >> !uint_p]
                ;
            typeUnsigned
                =	!(BIG_ENDIAN | LITTLE_ENDIAN) >> lexeme_d[UNSIGNED >> !range_p('1', '8')]
                ;

            attributeList
                = discard_node_d[LBRACE]
                >> lexeme_d[leaf_node_d[+(anychar_p - RBRACE)]]
                >> discard_node_d[RBRACE]
                ;

            other
                = otherSemi
                | otherEof
                ;
            otherSemi
                =	lexeme_d[leaf_node_d[+(anychar_p - SEMI)]]
                ;
            otherEol
                =	lexeme_d[leaf_node_d[+(anychar_p - eol_p)]]
                ;
            otherEof
                =	lexeme_d[leaf_node_d[+(anychar_p - end_p)]] 
                >>	end_p
                ;

        /*
            expression
                =   simpleExpression >>
                    ( relationalOperator >> simpleExpression
                    | empty
                    )
                ;

            relationalOperator
                =   EQUAL | NOT_EQUAL | GE | LE | LT | GT | IN_
                ;

            simpleExpression
                =   ( sign
                    | empty
                    ) >>
                    term >> *( addingOperator >> term )
                ;

            addingOperator
                =   PLUS | MINUS | OR
                ;

            term
                =   factor >> *( multiplyingOperator >> factor )
                ;

            multiplyingOperator
                =   STAR | SLASH | DIV | MOD | AND
                ;

            factor
                =   LPAREN >> expression >> RPAREN
                |   set
                |   longest_d[
                        variable
                    |   unsignedConstant
                    |   functionDesignator
                ]
                |   NOT >> factor
                ;

        */

#ifdef BOOST_SPIRIT_DEBUG
            BOOST_SPIRIT_DEBUG_RULE(program);
            BOOST_SPIRIT_DEBUG_RULE(query);
            BOOST_SPIRIT_DEBUG_RULE(comment);
            BOOST_SPIRIT_DEBUG_RULE(blockComment);
            BOOST_SPIRIT_DEBUG_RULE(lineComment);
            BOOST_SPIRIT_DEBUG_RULE(GrETLComment);
            BOOST_SPIRIT_DEBUG_RULE(GrETLContent);
            BOOST_SPIRIT_DEBUG_RULE(invokeComment);
            BOOST_SPIRIT_DEBUG_RULE(invoke);
            BOOST_SPIRIT_DEBUG_RULE(invokeLabel);
            BOOST_SPIRIT_DEBUG_RULE(commentContent);
            BOOST_SPIRIT_DEBUG_RULE(import);
            BOOST_SPIRIT_DEBUG_RULE(scopeFlag);
            BOOST_SPIRIT_DEBUG_RULE(selfFlag);
            BOOST_SPIRIT_DEBUG_RULE(module);
            BOOST_SPIRIT_DEBUG_RULE(moduleParams);
            BOOST_SPIRIT_DEBUG_RULE(moduleExport);
            BOOST_SPIRIT_DEBUG_RULE(otherModule);
            BOOST_SPIRIT_DEBUG_RULE(macro);
            BOOST_SPIRIT_DEBUG_RULE(macro_noparam);
            BOOST_SPIRIT_DEBUG_RULE(macro_emptyparam);
            BOOST_SPIRIT_DEBUG_RULE(macro_param);
            BOOST_SPIRIT_DEBUG_RULE(macroLabel);
            BOOST_SPIRIT_DEBUG_RULE(macroParams);
            BOOST_SPIRIT_DEBUG_RULE(macroContent);
            BOOST_SPIRIT_DEBUG_RULE(boolLiteral);
            BOOST_SPIRIT_DEBUG_RULE(emptySet);
            BOOST_SPIRIT_DEBUG_RULE(identifier);
            BOOST_SPIRIT_DEBUG_RULE(identifierReference);
            BOOST_SPIRIT_DEBUG_RULE(qualifiedIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(declAndAssign);
            BOOST_SPIRIT_DEBUG_RULE(declaration);
            BOOST_SPIRIT_DEBUG_RULE(assignment);
            BOOST_SPIRIT_DEBUG_RULE(record);
            BOOST_SPIRIT_DEBUG_RULE(recordBody);
            BOOST_SPIRIT_DEBUG_RULE(project);
            BOOST_SPIRIT_DEBUG_RULE(dataset);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormat);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatFlat);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsv);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsvAttr);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsvAttrHeading);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsvAttrMaxlength);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsvAttrSeparator);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsvAttrTerminator);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatCsvAttrValue);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatXml);
            BOOST_SPIRIT_DEBUG_RULE(datasetFormatXmlAttr);
            BOOST_SPIRIT_DEBUG_RULE(service);
            BOOST_SPIRIT_DEBUG_RULE(serviceDeclaration);
            BOOST_SPIRIT_DEBUG_RULE(paramaterizedAttribute);
            BOOST_SPIRIT_DEBUG_RULE(sizeOf);
            BOOST_SPIRIT_DEBUG_RULE(transform);
            BOOST_SPIRIT_DEBUG_RULE(cast);
            BOOST_SPIRIT_DEBUG_RULE(type);
            BOOST_SPIRIT_DEBUG_RULE(typeBoolean);
            BOOST_SPIRIT_DEBUG_RULE(typeInteger);
            BOOST_SPIRIT_DEBUG_RULE(typeReal);
            BOOST_SPIRIT_DEBUG_RULE(typeDecimal);
            BOOST_SPIRIT_DEBUG_RULE(typeUDecimal);
            BOOST_SPIRIT_DEBUG_RULE(typeString);
            BOOST_SPIRIT_DEBUG_RULE(typeQString);
            BOOST_SPIRIT_DEBUG_RULE(typeUnicode);
            BOOST_SPIRIT_DEBUG_RULE(typeData);
            BOOST_SPIRIT_DEBUG_RULE(typeVarString);
            BOOST_SPIRIT_DEBUG_RULE(typeVarUnicode);
            BOOST_SPIRIT_DEBUG_RULE(typeUnsigned);
            BOOST_SPIRIT_DEBUG_RULE(constParam);
            BOOST_SPIRIT_DEBUG_RULE(locale);
            BOOST_SPIRIT_DEBUG_RULE(other);
            BOOST_SPIRIT_DEBUG_RULE(otherSemi);
            BOOST_SPIRIT_DEBUG_RULE(otherEol);
            BOOST_SPIRIT_DEBUG_RULE(otherEof);
            BOOST_SPIRIT_DEBUG_RULE(attributeList);
            BOOST_SPIRIT_DEBUG_RULE(parameter);
            BOOST_SPIRIT_DEBUG_RULE(parameterList);
            BOOST_SPIRIT_DEBUG_RULE(stringLiteral);
            BOOST_SPIRIT_DEBUG_RULE(numericLiteral);
            BOOST_SPIRIT_DEBUG_RULE(value);
            BOOST_SPIRIT_DEBUG_RULE(valueOrDefault);
            BOOST_SPIRIT_DEBUG_RULE(valueList);
            BOOST_SPIRIT_DEBUG_RULE(functionDeclaration);
            BOOST_SPIRIT_DEBUG_RULE(selfIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(output);
#endif
        }

        symbols<> keywords;
        
        rule<ScannerT, parser_context<>, parser_tag<programID> > program;
        rule<ScannerT, parser_context<>, parser_tag<queryID> > query;
        rule<ScannerT, parser_context<>, parser_tag<commentID> > comment;
        rule<ScannerT, parser_context<>, parser_tag<GrETLCommentID> > GrETLComment;
        rule<ScannerT, parser_context<>, parser_tag<invokeCommentID> > invokeComment;
        rule<ScannerT, parser_context<>, parser_tag<blockCommentID> > blockComment;
        rule<ScannerT, parser_context<>, parser_tag<lineCommentID> > lineComment;
        rule<ScannerT, parser_context<>, parser_tag<GrETLContentID> > GrETLContent;
        rule<ScannerT, parser_context<>, parser_tag<invokeID> > invoke;
        rule<ScannerT, parser_context<>, parser_tag<invokeLabelID> > invokeLabel;
        rule<ScannerT, parser_context<>, parser_tag<commentContentID> > commentContent;
        rule<ScannerT, parser_context<>, parser_tag<importID> > import;
        rule<ScannerT, parser_context<>, parser_tag<scopeFlagID> > scopeFlag;
        rule<ScannerT, parser_context<>, parser_tag<selfFlagID> > selfFlag;
        rule<ScannerT, parser_context<>, parser_tag<moduleID> > module;
        rule<ScannerT, parser_context<>, parser_tag<moduleParamsID> > moduleParams;
        rule<ScannerT, parser_context<>, parser_tag<moduleExportID> > moduleExport;
        rule<ScannerT, parser_context<>, parser_tag<otherModuleID> > otherModule;
        rule<ScannerT, parser_context<>, parser_tag<macroID> > macro;
        rule<ScannerT, parser_context<>, parser_tag<macro_noparamID> > macro_noparam;
        rule<ScannerT, parser_context<>, parser_tag<macro_emptyparamID> > macro_emptyparam;
        rule<ScannerT, parser_context<>, parser_tag<macro_paramID> > macro_param;
        rule<ScannerT, parser_context<>, parser_tag<macroLabelID> > macroLabel;
        rule<ScannerT, parser_context<>, parser_tag<macroParamsID> > macroParams;
        rule<ScannerT, parser_context<>, parser_tag<macroContentID> > macroContent;
        rule<ScannerT, parser_context<>, parser_tag<boolLiteralID> > boolLiteral;
        rule<ScannerT, parser_context<>, parser_tag<emptySetID> > emptySet;
        rule<ScannerT, parser_context<>, parser_tag<identifierID> > identifier;
        rule<ScannerT, parser_context<>, parser_tag<qualifiedIdentifierID> > qualifiedIdentifier;
        rule<ScannerT, parser_context<>, parser_tag<declarationID> > declaration;
        rule<ScannerT, parser_context<>, parser_tag<assignmentID> > assignment;
        rule<ScannerT, parser_context<>, parser_tag<declAndAssignID> > declAndAssign;
        rule<ScannerT, parser_context<>, parser_tag<serviceDeclarationID> > serviceDeclaration;
        rule<ScannerT, parser_context<>, parser_tag<serviceID> > service;
        rule<ScannerT, parser_context<>, parser_tag<sizeOfID> > sizeOf;
        rule<ScannerT, parser_context<>, parser_tag<typeID> > type;
        rule<ScannerT, parser_context<>, parser_tag<castID> > cast;
        rule<ScannerT, parser_context<>, parser_tag<typeBooleanID> > typeBoolean;
        rule<ScannerT, parser_context<>, parser_tag<typeIntegerID> > typeInteger;
        rule<ScannerT, parser_context<>, parser_tag<typeRealID> > typeReal;
        rule<ScannerT, parser_context<>, parser_tag<typeDecimalID> > typeDecimal;
        rule<ScannerT, parser_context<>, parser_tag<typeUDecimalID> > typeUDecimal;
        rule<ScannerT, parser_context<>, parser_tag<typeStringID> > typeString;
        rule<ScannerT, parser_context<>, parser_tag<typeQStringID> > typeQString;
        rule<ScannerT, parser_context<>, parser_tag<typeUnicodeID> > typeUnicode;
        rule<ScannerT, parser_context<>, parser_tag<typeDataID> > typeData;
        rule<ScannerT, parser_context<>, parser_tag<typeVarStringID> > typeVarString;
        rule<ScannerT, parser_context<>, parser_tag<typeVarUnicodeID> > typeVarUnicode;
        rule<ScannerT, parser_context<>, parser_tag<typeUnsignedID> > typeUnsigned;
        rule<ScannerT, parser_context<>, parser_tag<localeID> > locale;
        rule<ScannerT, parser_context<>, parser_tag<recordID> > record;
        rule<ScannerT, parser_context<>, parser_tag<recordBodyID> > recordBody;
        rule<ScannerT, parser_context<>, parser_tag<valueID> > value;
        rule<ScannerT, parser_context<>, parser_tag<valueOrDefaultID> > valueOrDefault;
        rule<ScannerT, parser_context<>, parser_tag<valueListID> > valueList;
        rule<ScannerT, parser_context<>, parser_tag<constParamID> > constParam;
        rule<ScannerT, parser_context<>, parser_tag<parameterID> > parameter;
        rule<ScannerT, parser_context<>, parser_tag<parameterListID> > parameterList;
        rule<ScannerT, parser_context<>, parser_tag<functionDeclarationID> > functionDeclaration;
        rule<ScannerT, parser_context<>, parser_tag<identifierReferenceID> > identifierReference;
        rule<ScannerT, parser_context<>, parser_tag<selfIdentifierID> > selfIdentifier;
        rule<ScannerT, parser_context<>, parser_tag<projectID> > project;
        rule<ScannerT, parser_context<>, parser_tag<datasetID> > dataset;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatID> > datasetFormat;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatFlatID> > datasetFormatFlat;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvID> > datasetFormatCsv;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvAttrID> > datasetFormatCsvAttr;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvAttrHeadingID> > datasetFormatCsvAttrHeading;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvAttrMaxlengthID> > datasetFormatCsvAttrMaxlength;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvAttrSeparatorID> > datasetFormatCsvAttrSeparator;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvAttrTerminatorID> > datasetFormatCsvAttrTerminator;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatCsvAttrValueID> > datasetFormatCsvAttrValue;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatXmlID> > datasetFormatXml;
        rule<ScannerT, parser_context<>, parser_tag<datasetFormatXmlAttrID> > datasetFormatXmlAttr;
        rule<ScannerT, parser_context<>, parser_tag<paramaterizedAttributeID> > paramaterizedAttribute;
        rule<ScannerT, parser_context<>, parser_tag<transformID> > transform;
        rule<ScannerT, parser_context<>, parser_tag<attributeListID> > attributeList;
        rule<ScannerT, parser_context<>, parser_tag<otherID> > other;
        rule<ScannerT, parser_context<>, parser_tag<otherSemiID> > otherSemi;
        rule<ScannerT, parser_context<>, parser_tag<otherEolID> > otherEol;
        rule<ScannerT, parser_context<>, parser_tag<otherEofID> > otherEof;
        rule<ScannerT, parser_context<>, parser_tag<stringLiteralID> > stringLiteral;
        rule<ScannerT, parser_context<>, parser_tag<numericLiteralID> > numericLiteral;
        rule<ScannerT, parser_context<>, parser_tag<outputID> > output;
        rule<ScannerT, parser_context<>, parser_tag<programID> > const& start() const { return program; }
    };
};

void DoParse(const std::string & ecl, tree_parse_info<> &info);