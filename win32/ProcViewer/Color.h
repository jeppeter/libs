/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * Color.h - Interface declaration for Color
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-18
 */



#if !defined(AFX_OLOR_H__189A8D3B_D95F_42CA_86C2_5AF8C018D0D0__INCLUDED_)
#define AFX_OLOR_H__189A8D3B_D95F_42CA_86C2_5AF8C018D0D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorConstants.h"


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * Color - Manages manipulation of color.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-18
 */
class Color
{
    public:

	    explicit Color(); // Default constructor
        explicit Color( const COLORREF clrColor_i ); //Helper constructor
        explicit Color( const Color& clColor_i ); //Copy constructor

        // Destructor
	    virtual ~Color();

        // COLORREF operator overloaded
        operator COLORREF() const
        {
            return m_clrColor;
        }

        // Assignment operator overloaded
        Color& operator =( const Color& clColor_i )
        {
            if( this != &clColor_i )
            {
                m_clrColor = clColor_i.GetColorValue();
            }

            return *this;
        }

        // Assignment operator overloaded for COLORREF
        Color& operator = ( const COLORREF clrColor_i )
        {
            m_clrColor = clrColor_i;
            return *this;
        }

        // Returns red value stored in color
        BYTE GetRed() const{ return GetRValue( m_clrColor ); }

        // Returns green value stored in color
        BYTE GetGreen() const{ return GetGValue( m_clrColor ); }

        // Return blue value stored in color
        BYTE GetBlue() const{ return GetBValue( m_clrColor ); }

        // Set red
        void SetRed( const BYTE byRedToSet_i )
        { 
            m_clrColor = ( 0xff000000 | ( GetBlue() << 16) | ( GetGreen() << 8 ) | ( byRedToSet_i << 0 ));
        }

        // Set green attribute
        void SetGreen( const BYTE byGreenToSet_i )
        {
            m_clrColor = ( 0xff000000 | ( GetBlue() << 16) | ( byGreenToSet_i << 8 ) | ( GetRed() << 0 ));
        }

        // Set blue attribute
        void SetBlue( const BYTE byBlueToSet_i )
        {
            m_clrColor = ( 0xff000000 | ( byBlueToSet_i << 16) | ( GetGreen() << 8 ) | ( GetRed() << 0 ));
        }

        // Set components of color
        void SetColorComponents( const BYTE byRedToSet_i, 
                                 const BYTE byGreenToSet_i, 
                                 const BYTE byBlueToSet_i );

        // Make color brighter
        void Brighter();

        // Makes color darker
        void Darker();


       /** 
        * 
        * Returns internal color value.
        * 
        * @param       Nil
        * @return      COLORREF - Return color value
        * @exception   Nil
        * @see         Nil
        * @since       1.0
        */
        COLORREF GetColorValue() const
        {
            return m_clrColor;
        }

    private:

        // Color variable
        COLORREF m_clrColor;

        // Arbitrary decider value
        static const double DECIDER;
        
};// End Color

#endif // !defined(AFX_OLOR_H__189A8D3B_D95F_42CA_86C2_5AF8C018D0D0__INCLUDED_)
