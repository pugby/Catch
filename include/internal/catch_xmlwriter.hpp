/*
 *  catch_xmlwriter.hpp
 *  Catch
 *
 *  Created by Phil on 09/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED

#include <sstream>
#include <string>
#include <vector>

namespace Catch
{
    class XmlWriter
    {
    public:
        
        class ScopedElement
        {
        public:
            ///////////////////////////////////////////////////////////////////
            ScopedElement
            (
                XmlWriter* writer
            )
            :   m_writer( writer )
            {
            }
            
            ///////////////////////////////////////////////////////////////////
            ScopedElement
            (
                const ScopedElement& other
            )
            :   m_writer( other.m_writer )
            {
                other.m_writer = NULL;
            }
            
            ///////////////////////////////////////////////////////////////////
            ~ScopedElement
            ()
            {
                if( m_writer )
                    m_writer->endElement();
            }

            ///////////////////////////////////////////////////////////////////
            ScopedElement& writeText
            (
                const std::string& text
            )
            {
                m_writer->writeText( text );
                return *this;
            }

            ///////////////////////////////////////////////////////////////////
            template<typename T>
            ScopedElement& writeAttribute
            (
                const std::string& name,
                const T& attribute
            )
            {
                m_writer->writeAttribute( name, attribute );
                return *this;
            }
            
        private:
            mutable XmlWriter* m_writer;
        };
        
        ///////////////////////////////////////////////////////////////////////
        XmlWriter
        ()
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &std::cout )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter
        (
            std::ostream& os
        )
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &os )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////
        ~XmlWriter
        ()
        {
            while( !m_tags.empty() )
            {
                endElement();
            }
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& operator = 
        (
            const XmlWriter& other
        )
        {
            XmlWriter temp( other );
            swap( temp );
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        void swap
        (
            XmlWriter& other
        )
        {
            std::swap( m_tagIsOpen, other.m_tagIsOpen );
            std::swap( m_needsNewline, other.m_needsNewline );
            std::swap( m_tags, other.m_tags );
            std::swap( m_indent, other.m_indent );
            std::swap( m_os, other.m_os );
        }
        
        ///////////////////////////////////////////////////////////////////////
        XmlWriter& startElement
        (
            const std::string& name
        )
        {
            ensureTagClosed();
            newlineIfNecessary();
            stream() << m_indent << "<" << name;
            m_tags.push_back( name );
            m_indent += "  ";
            m_tagIsOpen = true;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        ScopedElement scopedElement
        (
            const std::string& name
        )
        {
            ScopedElement scoped( this );
            startElement( name );
            return scoped;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& endElement
        ()
        {
            newlineIfNecessary();
            m_indent = m_indent.substr( 0, m_indent.size()-2 );
            if( m_tagIsOpen )
            {
                stream() << "/>\n";
                m_tagIsOpen = false;
            }
            else
            {
                stream() << m_indent << "</" << m_tags.back() << ">\n";
            } 
            m_tags.pop_back();
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeAttribute
        (
            const std::string& name, 
            const std::string& attribute
        )
        {
            if( !name.empty() && !attribute.empty() )
            {
                stream() << " " << name << "=\"";
                writeEncodedText( attribute );
                stream() << "\"";
            }
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeAttribute
        (
            const std::string& name, 
            bool attribute
        )
        {
            stream() << " " << name << "=\"" << ( attribute ? "true" : "false" ) << "\"";
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        template<typename T>
        XmlWriter& writeAttribute
        (
            const std::string& name, 
            const T& attribute
        )
        {
            if( !name.empty() )
            {
                stream() << " " << name << "=\"" << attribute << "\"";
            }
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeText
        (
            const std::string& text
        )
        {
            if( !text.empty() )
            {
                bool tagWasOpen = m_tagIsOpen;
                ensureTagClosed();
                if( tagWasOpen )
                    stream() << m_indent;
                writeEncodedText( text );
                m_needsNewline = true;
            }
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeComment
        (
            const std::string& text 
        )
        {
            ensureTagClosed();
            stream() << m_indent << "<!--" << text << "-->";
            m_needsNewline = true;
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeBlankLine
        ()
        {
            ensureTagClosed();
            stream() << "\n";
            return *this;
        }
        
    private:
        
        ///////////////////////////////////////////////////////////////////////
        std::ostream& stream
        ()
        {
            return *m_os;
        }
        
        ///////////////////////////////////////////////////////////////////////
        void ensureTagClosed
        ()
        {
            if( m_tagIsOpen )
            {
                stream() << ">\n";
                m_tagIsOpen = false;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////
        void newlineIfNecessary
        ()
        {
            if( m_needsNewline )
            {
                stream() << "\n";
                m_needsNewline = false;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////
        void writeEncodedText
        (
            const std::string& text
        )
        {
            // !TBD finish this
            if( !findReplaceableString( text, "<", "&lt;" ) &&
               !findReplaceableString( text, "&", "&amp;" ) &&
               !findReplaceableString( text, "\"", "&quote;" ) )
            {
                stream() << text;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////
        bool findReplaceableString
        (
            const std::string& text, 
            const std::string& replaceWhat, 
            const std::string& replaceWith
        )
        {
            std::string::size_type pos = text.find_first_of( replaceWhat );
            if( pos != std::string::npos )
            {
                stream() << text.substr( 0, pos ) << replaceWith;
                writeEncodedText( text.substr( pos+1 ) );
                return true;
            }
            return false;
        }
        
        bool m_tagIsOpen;
        bool m_needsNewline;
        std::vector<std::string> m_tags;
        std::string m_indent;
        std::ostream* m_os;
    };
    
}
#endif // TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED
