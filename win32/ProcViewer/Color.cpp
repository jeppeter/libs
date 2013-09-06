/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * Color.cpp - Implementation for Color
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-18
 */


#include "stdafx.h"
#include "Color.h"


// Static variable initialization
const double Color::DECIDER = 0.7;


/** 
 * 
 * Default constructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Color::Color() : m_clrColor( 0 )
{}


/** 
 * 
 * Helper constructor.
 * 
 * @param       clrColor_i - Color to assign
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Color::Color( const COLORREF clrColor_i ) : m_clrColor( clrColor_i )
{}


/** 
 * 
 * 
 * 
 * @param       clColor_i - 
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Color::Color( const Color& clColor_i ) : m_clrColor( clColor_i.m_clrColor )
{}


/** 
 * 
 * Destructor does nothing
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Color::~Color()
{}


/** 
 * 
 * Sets individual color components at one go.
 * 
 * @param       byRedToSet_i   - Red to set
 * @param       byGreenToSet_i - Green to set
 * @param       byBlueToSet_i  - Blue to set
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Color::SetColorComponents( const BYTE byRedToSet_i, 
                                const BYTE byGreenToSet_i, 
                                const BYTE byBlueToSet_i )
{
    // Set colors
    SetRed( byRedToSet_i );
    SetGreen( byGreenToSet_i );
    SetBlue( byBlueToSet_i );
}


/** 
 * 
 * Makes color brighter
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Color::Brighter()
{
    // Get color components
    int nRed = GetRed();
    int nGreen = GetGreen();
    int nBlue = GetBlue();

    const int nFactor = (int )( 1.0 / ( 1.0 - DECIDER ));

    // If color invalid then return
    if( nRed == 0 && nBlue == 0 && nGreen == 0 )
    {
        SetColorComponents( SCAST( BYTE, nFactor ), 
                            SCAST( BYTE, nFactor ), 
                            SCAST( BYTE, nFactor ));

        return;
    }

    // Prepare red
    if( nRed > 0 && nRed < nFactor ) 
    {
        nRed = nFactor;
    }

    // Prepare green
    if( nGreen > 0 && nGreen < nFactor )
    {
        nGreen = nFactor;
    }

    // Prepare blue
    if( nBlue > 0 && nBlue < nFactor )
    {
        nBlue = nFactor;
    }

    // Prepare a brighter combination
    nRed = SCAST( int, nRed / DECIDER );
    nGreen = SCAST( int, nGreen / DECIDER );
    nBlue = SCAST( int, nBlue / DECIDER );

    // Set new color
    SetColorComponents( SCAST( BYTE, min( nRed, 255 )),
                        SCAST( BYTE, min( nGreen, 255 )),
                        SCAST( BYTE, min( nBlue, 255 )));
}// End Brighter


/** 
 * 
 * Darkens color
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Color::Darker()
{
    // Prepare RGB
    int nRed = SCAST( int, GetRed() * DECIDER );
    int nGreen = SCAST( int, GetGreen() * DECIDER );
    int nBlue = SCAST( int, GetBlue() * DECIDER );

    SetColorComponents( SCAST( BYTE, max( nRed, 0 )),
                        SCAST( BYTE, max( nGreen, 0 )),
                        SCAST( BYTE, max( nBlue, 0 )));
}