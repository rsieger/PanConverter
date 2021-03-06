/* SOCATv5 converter        */
/* 2017-07-04               */
/* Dr. Rainer Sieger        */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-04-07

int MainWindow::convertSOCATv5( const QString &s_FilenameIn, const QString &s_FilenameOut, const int i_CodecInput, const int i_CodecOutput, const int i_EOL, const int i_NumOfFiles )
{
    int         i                       = 0;
    int         n                       = 0;
    int         s                       = 0;

    int         stopProgress            = 0;

    QString     s_EOL                   = setEOLChar( i_EOL );

    QStringList sl_Input;

// **********************************************************************************************
// read file

    if ( ( n = readFile( s_FilenameIn, sl_Input, i_CodecInput ) ) < 1 )
        return( n );

// **********************************************************************************************
// open output file

    QFile fdata( s_FilenameOut );

    if ( fdata.open( QIODevice::WriteOnly | QIODevice::Text) == false )
        return( -20 );

    QTextStream tdata( &fdata );

    switch ( i_CodecOutput )
    {
    case _SYSTEM_:
        break;
    case _LATIN1_:
        tdata.setCodec( QTextCodec::codecForName( "ISO 8859-1" ) );
        break;
    case _APPLEROMAN_:
        tdata.setCodec( QTextCodec::codecForName( "Apple Roman" ) );
        break;
    default:
        tdata.setCodec( QTextCodec::codecForName( "UTF-8" ) );
        break;
    }

// **********************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Converting SOCAT data..." ), sl_Input.count() );

// **********************************************************************************************
// Skip SOCAT Metadata

    bool b_EndOfMetadataReached  = false;

    while ( ( b_EndOfMetadataReached == false ) && ( i < n ) && ( stopProgress != _APPBREAK_ ) )
    {
        if ( sl_Input.at( i ).section( "\t", 2, 2 ) != "SOCAT_DOI" )
        {
            ++i;
            stopProgress = incProgress( i_NumOfFiles, i );
        }
        else
        {
            s = ++i;
            b_EndOfMetadataReached = true;
        }
    }

// **********************************************************************************************
// create data file

    tdata << "Event label\t";                                                                                                       // 0
    tdata << "Date/Time\t";                                                                                                         // 4, 5, 6, 7, 8, 9

    tdata << "Latitude\t";                                                                                                          // 11
    tdata << "Longitude\t";                                                                                                         // 10

    tdata << "Depth, water [m]\t";                                                                                                  // 12

    tdata << "Salinity []\t";                                                                                                       // 13
    tdata << "Temperature, water [deg C]\t";                                                                                        // 14

    tdata << "Temperature at equilibration [deg C]\t";                                                                              // 15
    tdata << "Pressure, atmospheric [hPa]\t";                                                                                       // 16
    tdata << "Pressure at equilibration [hPa]\t";                                                                                   // 17

    tdata << "Salinity, interpolated []@WOA\t";                                                                                     // 18
    tdata << "Pressure, atmospheric, interpolated [hPa]@NCEP\t";                                                                    // 19
    tdata << "Depth, bathymetric, interpolated/gridded [m]@ETOPO\t";                                                                        // 20
    tdata << "Distance [km]@d2l, estimated distance to major land mass\t";                                                          // 21
    tdata << "xCO2 (air), interpolated [mymol/mol]@GVCO2\t";                                                                        // 22

    tdata << "xCO2 (water) at equilibrator temperature (dry air) [mymol/mol]\t";                                                    // 23
    tdata << "xCO2 (water) at sea surface temperature (dry air) [mymol/mol]\t";                                                     // 24

    tdata << "Partial pressure of CO2 (water) at equilibrator temperature (wet air) [myatm]\t";                                     // 25
    tdata << "Partial pressure of CO2 (water) at sea surface temperature (wet air) [myatm]\t";                                      // 26

    tdata << "Fugacity of CO2 (water) at equilibrator temperature (wet air) [myatm]\t";                                             // 27
    tdata << "Fugacity of CO2 (water) at sea surface temperature (wet air) [myatm]\t";                                              // 28

    tdata << "Fugacity of CO2 (water) at sea surface temperature (wet air) [myatm]@Recomputed after SOCAT (Pfeil et al., 2011)\t";  // 29
    tdata << "Algorithm []@fCO2rec_src, Algorithm for generating fCO2rec from the raw data, 0:not generated\t";                     // 30
    tdata << "Quality flag [#]@WOCE quality flag for fCO2rec: 2:good, 3:questionable, 4:bad, 9:not generated";                      // 31

    tdata  << s_EOL;

    while ( ( i < n ) && ( stopProgress != _APPBREAK_ ) )
    {
        QString s_Input  = "";
        QString s_Output = "";

        s_Input  = sl_Input.at( i );
        s_Input  = s_Input.replace( "NaN", "" );
        s_Output = s_Input.section( "\t", 0, 0 ) + "-track\t";

        s_Output.append( createDateTimeOutput( s_Input )  + "\t" ) ;

        if ( s_Input.section( "\t", 11, 11 ).isEmpty() == false ) // Latitude
        {
            float f_Latitude = s_Input.section( "\t", 11, 11 ).toFloat();

            if ( ( f_Latitude <= 90. ) && ( f_Latitude >= -90. ) )
                s_Output.append( QString( "%1" ).arg( f_Latitude, 0, 'f', 4 ) + "\t" );
            else
                s_Output.append( "\t" );
        }
        else
            s_Output.append( "\t" );

        if ( s_Input.section( "\t", 10, 10 ).isEmpty() == false ) // Longitude
        {
            float f_Longitude = s_Input.section( "\t", 10, 10 ).toFloat();

            if ( ( f_Longitude <= 360. ) && ( f_Longitude >= 180. ) )
                f_Longitude -= 360.;

            if ( ( f_Longitude <= 180. ) && ( f_Longitude >= -180. ) )
                s_Output.append( QString( "%1" ).arg( f_Longitude, 0, 'f', 4 ) + "\t" );
            else
                s_Output.append( "\t" );
        }
        else
            s_Output.append( "\t" );

        if ( s_Input.section( "\t", 12, 12 ).isEmpty() == false ) // Depth, water
            s_Output.append( QString( "%1" ).arg( s_Input.section( "\t", 12, 12 ).toFloat(), 0, 'f', 0 ) + "\t" );
        else
            s_Output.append( "0\t" );

        s_Output.append( s_Input.section( "\t", 13, 31 ) );

        tdata << s_Output << s_EOL;

        stopProgress = incProgress( i_NumOfFiles, ++i );
    }

    fdata.close();

    resetProgress( i_NumOfFiles );

// **********************************************************************************************

    createEventImportFile( sl_Input.at( s ), sl_Input.at( n-1 ), s_FilenameOut, i_CodecOutput, i_EOL );

// **********************************************************************************************

    if ( stopProgress == _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2017-07-05

int MainWindow::createEventImportFile( const QString &s_First, const QString &s_Last, const QString &s_FilenameOut, const int i_CodecOutput, const int i_EOL )
{
    QString s_EOL    = setEOLChar( i_EOL );

    QString s_Input  = s_First;
    QString s_Output = s_Input.section( "\t", 0, 0 ) + "\t";

    QDate   date;
    QTime   time;

// **********************************************************************************************
// open event import file

    QFileInfo fi( s_FilenameOut );

    QFile fevent( fi.absolutePath() + "/Event_imp_" + fi.completeBaseName() + "." + fi.suffix() );

    if ( fevent.open( QIODevice::WriteOnly | QIODevice::Text) == false )
        return( -20 );

    QTextStream tevent( &fevent );

    switch ( i_CodecOutput )
    {
    case _SYSTEM_:
        break;
    case _LATIN1_:
        tevent.setCodec( QTextCodec::codecForName( "ISO 8859-1" ) );
        break;
    case _APPLEROMAN_:
        tevent.setCodec( QTextCodec::codecForName( "Apple Roman" ) );
        break;
    default:
        tevent.setCodec( QTextCodec::codecForName( "UTF-8" ) );
        break;
    }

// **********************************************************************************************

    tevent << "Campaign\tLabelEvent\tGear\tDateTimeEvent\tLatitudeEvent\tLongitudeEvent\tDateTimeEvent2\tLatitudeEvent2\tLongitudeEvent2" << s_EOL;

// **********************************************************************************************

    s_Input = s_Input.replace( "NaN", "" );

    s_Output.append( s_Input.section( "\t", 0, 0 ) + "-track" + "\t" );
    s_Output.append( "CT\t" );

    s_Output.append( createDateTimeOutput( s_Input )  + "\t" ) ;

    if ( s_Input.section( "\t", 11, 11 ).isEmpty() == false ) // Latitude
    {
        float f_Latitude = s_Input.section( "\t", 11, 11 ).toFloat();

        if ( ( f_Latitude <= 90. ) && ( f_Latitude >= -90. ) )
            s_Output.append( QString( "%1" ).arg( f_Latitude, 0, 'f', 4 ) + "\t" );
        else
            s_Output.append( "\t" );
    }
    else
        s_Output.append( "\t" );

    if ( s_Input.section( "\t", 10, 10 ).isEmpty() == false ) // Longitude
    {
        float f_Longitude = s_Input.section( "\t", 10, 10 ).toFloat();

        if ( ( f_Longitude <= 360. ) && ( f_Longitude >= 180. ) )
            f_Longitude -= 360.;

        if ( ( f_Longitude <= 180. ) && ( f_Longitude >= -180. ) )
            s_Output.append( QString( "%1" ).arg( f_Longitude, 0, 'f', 4 ) + "\t" );
        else
            s_Output.append( "\t" );
    }
    else
        s_Output.append( "\t" );

// **********************************************************************************************

    s_Input  = s_Last;
    s_Input  = s_Input.replace( "NaN", "" );

    s_Output.append( createDateTimeOutput( s_Input )  + "\t" ) ;

    if ( s_Input.section( "\t", 11, 11 ).isEmpty() == false ) // Latitude
    {
        float f_Latitude = s_Input.section( "\t", 11, 11 ).toFloat();

        if ( ( f_Latitude <= 90. ) && ( f_Latitude >= -90. ) )
            s_Output.append( QString( "%1" ).arg( f_Latitude, 0, 'f', 4 ) + "\t" );
        else
            s_Output.append( "\t" );
    }
    else
        s_Output.append( "\t" );

    if ( s_Input.section( "\t", 10, 10 ).isEmpty() == false ) // Longitude
    {
        float f_Longitude = s_Input.section( "\t", 10, 10 ).toFloat();

        if ( ( f_Longitude <= 360. ) && ( f_Longitude >= 180. ) )
            f_Longitude -= 360.;

        if ( ( f_Longitude <= 180. ) && ( f_Longitude >= -180. ) )
            s_Output.append( QString( "%1" ).arg( f_Longitude, 0, 'f', 4 ) + "\t" );
        else
            s_Output.append( "\t" );
    }
    else
        s_Output.append( "\t" );

    tevent << s_Output << s_EOL;

    fevent.close();

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2017-07-27

QString MainWindow::createDateTimeOutput( const QString s_Input )
{
    QString s_Output = "";

    QDate date = QDate( s_Input.section( "\t", 4, 4 ).toInt(), s_Input.section( "\t", 5, 5 ).toInt(), s_Input.section( "\t", 6, 6 ).toInt() );
    QTime time = QTime( s_Input.section( "\t", 7, 7 ).toInt(), s_Input.section( "\t", 8, 8 ).toInt(), s_Input.section( "\t", 9, 9 ).toFloat(), 0 );

// **********************************************************************************************

    if ( ( date.isValid() == true ) && ( time.isValid() == true ) )
    {
        s_Output.append( date.toString( "yyyy-MM-dd" ) + "T" + time.toString( "hh:mm:ss" ) );
    }
    else
    {
        // Second chance if something wrong
        int   i_Year   = s_Input.section( "\t", 4, 4 ).toInt();
        int   i_Month  = s_Input.section( "\t", 5, 5 ).toInt();
        int   i_Day    = s_Input.section( "\t", 6, 6 ).toInt();
        int   i_Hour   = s_Input.section( "\t", 7, 7 ).toInt();
        int   i_Minute = s_Input.section( "\t", 8, 8 ).toInt();
        float f_Second = s_Input.section( "\t", 9, 9 ).toFloat();

        if ( f_Second > 59.999 )
        {
            f_Second -= 60.;
            i_Minute += 1;
        }

        if ( i_Minute > 59 )
        {
            i_Minute -= 60;
            i_Hour   += 1;
        }

        if ( i_Hour > 23 )
        {
            i_Hour -= 24;
            i_Day  += 1;
        }

        QDate date = QDate( i_Year, i_Month, i_Day );
        QTime time = QTime( i_Hour, i_Minute, f_Second, 0 );

        if ( ( date.isValid() == true ) && ( time.isValid() == true ) )
        {
            s_Output.append( date.toString( "yyyy-MM-dd" ) + "T" + time.toString( "hh:mm:ss" ) );
        }
        else
        {
            s_Output.append( tr( "wrong date and/or time" ) );
        }
    }

    return( s_Output );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-04-07

void MainWindow::doConvertSOCATv5()
{
    int     i               = 0;
    int     err             = 0;
    int     stopProgress    = 0;

    QString s_FilenameIn    = "";
    QString s_FilenameOut   = "";

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Converting SOCAT data..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            if ( buildFilename( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList.at( i ), s_FilenameIn, s_FilenameOut ) == true )
            {
                err = convertSOCATv5( s_FilenameIn, s_FilenameOut, gi_CodecInput, gi_CodecOutput, gi_EOL, gsl_FilenameList.count() );

                stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
            }
            else
            {
                err = _FILENOTEXISTS_;
            }
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Converting SOCAT data was canceled" ) );

    onError( err );
}

