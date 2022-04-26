#include "dataValue.h"
#include "dataTypeUtil.h"
#include "myException.h"
using namespace std;




void DataValue::doIntergerPromotion(E_DataType promotionTp)
{
    if ( promotionTp == E_TP_S_INT ) {
        switch( type ) 
        {
        case E_TP_CHAR:
            value.sint_val = static_cast<signed int>( value.char_val );
            break;
        case E_TP_U_CHAR:
            value.sint_val = static_cast<signed int>( value.uchar_val );
            break;
        case E_TP_S_CHAR:
            value.sint_val = static_cast<signed int>( value.schar_val );
            break;
        case E_TP_U_SHORT:
            value.sint_val = static_cast<signed int>( value.ushort_val );
            break;
        case E_TP_S_SHORT:
            value.sint_val = static_cast<signed int>( value.sshort_val );
            break;
        default:
            break;
        }

        type = promotionTp;
    } else if ( promotionTp == E_TP_U_INT ) {
        switch( type ) 
        {
        case E_TP_CHAR:
            value.uint_val = static_cast<unsigned int>( value.char_val );
            break;
        case E_TP_U_CHAR:
            value.uint_val = static_cast<unsigned int>( value.uchar_val );
            break;
        case E_TP_S_CHAR:
            value.uint_val = static_cast<unsigned int>( value.schar_val );
            break;
        case E_TP_U_SHORT:
            value.uint_val = static_cast<unsigned int>( value.ushort_val );
            break;
        case E_TP_S_SHORT:
            value.uint_val = static_cast<unsigned int>( value.sshort_val );
            break;
        default:
            break;
        }

        type = promotionTp;
    } else {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

}


void DataValue::doConvertion(E_DataType destinationTp)
{

    switch( destinationTp )
    {
    case E_TP_U_INT:
        {
            switch( type )
            {
            case E_TP_U_INT:
                // Same , Do Nothing
                break;
            case E_TP_S_INT:
                value.uint_val = static_cast<unsigned int>( value.sint_val );
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }
        }
        break;
    case E_TP_S_INT:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.sint_val = static_cast<signed int>( value.uint_val );
                break;
            case E_TP_S_INT:
                // Same , Do Nothing
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }
        }
        break;
    case E_TP_U_LONG:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.ulong_val = static_cast<unsigned long>( value.uint_val );
                break;
            case E_TP_S_INT:
                value.ulong_val = static_cast<unsigned long>( value.sint_val );
                break;
            case E_TP_U_LONG:
                // Same , Do Nothing
                break;
            case E_TP_S_LONG:
                value.ulong_val = static_cast<unsigned long>( value.slong_val );
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }
        }
        break;
    case E_TP_S_LONG:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.slong_val = static_cast<signed long>( value.uint_val );
                break;
            case E_TP_S_INT:
                value.slong_val = static_cast<signed long>( value.sint_val );
                break;
            case E_TP_U_LONG:
                value.slong_val = static_cast<signed long>( value.ulong_val );
                break;
            case E_TP_S_LONG:
                // Same , Do Nothing
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }
        }
        break;
    case E_TP_U_LONG_LONG:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.ulonglong_val = static_cast<unsigned long long>( value.uint_val );
                break;
            case E_TP_S_INT:
                value.ulonglong_val = static_cast<unsigned long long>( value.sint_val );
                break;
            case E_TP_U_LONG:
                value.ulonglong_val = static_cast<unsigned long long>( value.ulong_val );
                break;
            case E_TP_S_LONG:
                value.ulonglong_val = static_cast<unsigned long long>( value.slong_val );
                break;
            case E_TP_U_LONG_LONG:
                // Same , Do Nothing
                break;
            case E_TP_S_LONG_LONG:
                value.ulonglong_val = static_cast<unsigned long long>( value.slonglong_val );
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }
        }
        break;
    case E_TP_S_LONG_LONG:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.slonglong_val = static_cast<signed long long>( value.uint_val );
                break;
            case E_TP_S_INT:
                value.slonglong_val = static_cast<signed long long>( value.sint_val );
                break;
            case E_TP_U_LONG:
                value.slonglong_val = static_cast<signed long long>( value.ulong_val );
                break;
            case E_TP_S_LONG:
                value.slonglong_val = static_cast<signed long long>( value.slong_val );
                break;
            case E_TP_U_LONG_LONG:
                value.slonglong_val = static_cast<signed long long>( value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                // Same , Do Nothing
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }
            
        }
        break;
    case E_TP_FLOAT:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.float_val = static_cast<float>( value.uint_val );
                break;
            case E_TP_S_INT:
                value.float_val = static_cast<float>( value.sint_val );
                break;
            case E_TP_U_LONG:
                value.float_val = static_cast<float>( value.ulong_val );
                break;
            case E_TP_S_LONG:
                value.float_val = static_cast<float>( value.slong_val );
                break;
            case E_TP_U_LONG_LONG:
                value.float_val = static_cast<float>( value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                value.float_val = static_cast<float>( value.slonglong_val );
                break;
            case E_TP_FLOAT:
                // Same , Do Nothing
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }

        }
        break;
    case E_TP_DOUBLE:
        {
            switch( type )
            {
            case E_TP_U_INT:
                value.double_val = static_cast<double>( value.uint_val );
                break;
            case E_TP_S_INT:
                value.double_val = static_cast<double>( value.sint_val );
                break;
            case E_TP_U_LONG:
                value.double_val = static_cast<double>( value.ulong_val );
                break;
            case E_TP_S_LONG:
                value.double_val = static_cast<double>( value.slong_val );
                break;
            case E_TP_U_LONG_LONG:
                value.double_val = static_cast<double>( value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                value.double_val = static_cast<double>( value.slonglong_val );
                break;
            case E_TP_FLOAT:
                value.double_val = static_cast<double>( value.float_val );
                break;
            case E_TP_DOUBLE:
                // Same , Do Nothing
                break;
            default:
                {
                    MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

                    std::string detail( __FILE__ );
                    detail += (string(" @line : ") + to_string(__LINE__));

                    e.setDetail(detail);
                    throw e;
                }
                break;
            }

        }
        break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }


    type = destinationTp;
}



void DataValue::downerCast(E_DataType castTp)
{
    if ( castTp == this->type ) {
        return;
    }

    switch( castTp )
    {
    case E_TP_U_INT:
        {
            switch( this->type )
            {
            case E_TP_U_INT:
                // Do nothing
                break;
            case E_TP_S_INT:
                this->value.uint_val = static_cast<unsigned int>( this->value.sint_val );
                break;
            case E_TP_U_LONG:
                this->value.uint_val = static_cast<unsigned int>( this->value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.uint_val = static_cast<unsigned int>( this->value.slong_val );
                break;
            case E_TP_U_LONG_LONG:
                this->value.uint_val = static_cast<unsigned int>( this->value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.uint_val = static_cast<unsigned int>( this->value.slonglong_val );
                break;
            default:
                break;
            }

            this->type = E_TP_U_INT;
        }
        break;
    case E_TP_S_INT:
        {
            switch( this->type )
            {
            case E_TP_U_INT:
                this->value.sint_val = static_cast<signed int>( this->value.uint_val );
                break;
            case E_TP_S_INT:
                // Do nothing
                break;
            case E_TP_U_LONG:
                this->value.sint_val = static_cast<signed int>( this->value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.sint_val = static_cast<signed int>( this->value.slong_val );
                break;
            case E_TP_U_LONG_LONG:
                this->value.sint_val = static_cast<signed int>( this->value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.sint_val = static_cast<signed int>( this->value.slonglong_val );
                break;
            default:
                break;
            }

            this->type = E_TP_S_INT;
        }
        break;
    default:
        // Do Nothing
        break;
    }
}



DataValue DataValue::operator + () // positive
{
    DataValue retVal(*this);
    return retVal;
}

DataValue DataValue::operator - () // negative
{
    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = -(this->value.uint_val); 
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = -(this->value.sint_val);
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = -(this->value.ulong_val);
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = -(this->value.slong_val);
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = -(this->value.ulonglong_val); 
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = -(this->value.slonglong_val);
        break;
    case E_TP_FLOAT:
        retVal.value.float_val = -(this->value.float_val); 
        break;
    case E_TP_DOUBLE:
        retVal.value.double_val = -(this->value.double_val); 
        break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}



DataValue DataValue::operator + ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val + right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val + right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val + right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val + right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val + right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val + right.value.slonglong_val;
        break;
    case E_TP_FLOAT:
        retVal.value.float_val = this->value.float_val + right.value.float_val;
        break;
    case E_TP_DOUBLE:
        retVal.value.double_val = this->value.double_val + right.value.double_val;
        break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;
}

DataValue DataValue::operator - ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val - right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val - right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val - right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val - right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val - right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val - right.value.slonglong_val;
        break;
    case E_TP_FLOAT:
        retVal.value.float_val = this->value.float_val - right.value.float_val;
        break;
    case E_TP_DOUBLE:
        retVal.value.double_val = this->value.double_val - right.value.double_val;
        break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}

DataValue DataValue::operator * ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val * right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val * right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val * right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val * right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val * right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val * right.value.slonglong_val;
        break;
    case E_TP_FLOAT:
        retVal.value.float_val = this->value.float_val * right.value.float_val;
        break;
    case E_TP_DOUBLE:
        retVal.value.double_val = this->value.double_val * right.value.double_val;
        break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;
}

DataValue DataValue::operator / ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    auto pDtInfo = DataTypeUtil::getTypeInfo( right.type );
    if ( pDtInfo->isIntegerFamily() ) {
        if ( right.isIntZero() ) {
            MyException e(E_THROW_DIVIDE_ZERO);
            throw e;
        }
    }


    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val / right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val / right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val / right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val / right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val / right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val / right.value.slonglong_val;
        break;
    case E_TP_FLOAT:
        retVal.value.float_val = this->value.float_val / right.value.float_val;
        break;
    case E_TP_DOUBLE:
        retVal.value.double_val = this->value.double_val / right.value.double_val;
        break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;
}

DataValue DataValue::operator % ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT);
    //     e.setDetail("Can't apply  '%' operator on float operands");
    //     throw e;
    // }

    auto pDtInfo = DataTypeUtil::getTypeInfo( right.type );
    if ( pDtInfo->isIntegerFamily() ) {
        if ( right.isIntZero()  ) {
            MyException e(E_THROW_MODULO_ZERO);
            throw e;
        }
    }


    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val % right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val % right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val % right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val % right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val % right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val % right.value.slonglong_val;
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}


DataValue DataValue::operator & ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT);
    //     e.setDetail("Can't apply  '&' operator on float operands");
    //     throw e;
    // }

    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val & right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val & right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val & right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val & right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val & right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val & right.value.slonglong_val;
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}

DataValue DataValue::operator | ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT);
    //     e.setDetail("Can't apply  '|' operator on float operands");
    //     throw e;
    // }

    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val | right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val | right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val | right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val | right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val | right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val | right.value.slonglong_val;
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;
}

DataValue DataValue::operator ^ ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT);
    //     e.setDetail("Can't apply  '^' operator on float operands");
    //     throw e;
    // }


    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val ^ right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val ^ right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val ^ right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val ^ right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val ^ right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val ^ right.value.slonglong_val;
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}


bool DataValue::isIntZero() const
{
    auto bret = false;

    switch( this->type )
    {
    case E_TP_CHAR:
        bret = (this->value.char_val == 0);
        break;
    case E_TP_U_CHAR:
        bret = (this->value.uchar_val == 0);
        break;
    case E_TP_S_CHAR:
        bret = (this->value.schar_val == 0);
        break;
    case E_TP_U_SHORT:
        bret = (this->value.ushort_val == 0);
        break;
    case E_TP_S_SHORT:
        bret = (this->value.sshort_val == 0);
        break;

    case E_TP_U_INT:
        bret = (this->value.uint_val == 0);
        break;
    case E_TP_S_INT:
        bret = (this->value.sint_val == 0);
        break;
    case E_TP_U_LONG:
        bret = (this->value.ulong_val == 0);
        break;
    case E_TP_S_LONG:
        bret = (this->value.slong_val == 0);
        break;
    case E_TP_U_LONG_LONG:
        bret = (this->value.ulonglong_val == 0); 
        break;
    case E_TP_S_LONG_LONG:
        bret = (this->value.slonglong_val == 0);
        break;
    // case E_TP_FLOAT:
    //     retVal.value.float_val = -(this->value.float_val); 
    //     break;
    // case E_TP_DOUBLE:
    //     retVal.value.double_val = -(this->value.double_val); 
    //     break;
    default:
        break;
    }

    return bret;
}


bool DataValue::isFloatZero()
{
    auto bret = false;
    if ( this->type == E_TP_FLOAT ) {
        bret = ( (this->value.float_val - 0.0f) < 1e-5f);
    } else if ( this->type == E_TP_DOUBLE ) {
        bret = ( (this->value.double_val - 0.0) < 1e-5);
    }

    return bret;

}



DataValue DataValue::operator ~ () // bit not
{
    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT);
    //     throw e;
    // }


    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = ~(this->value.uint_val); 
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = ~(this->value.sint_val);
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = ~(this->value.ulong_val); 
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = ~(this->value.slong_val);
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = ~(this->value.ulonglong_val);
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = ~(this->value.slonglong_val); 
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}


DataValue DataValue::operator << ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }


    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT);
    //     throw e;
    // }

    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val << right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val << right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val << right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val << right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val << right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val << right.value.slonglong_val;
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;

}


DataValue DataValue::operator >> ( const DataValue& right)
{
    if ( this->type != right.type ) {
        MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

        std::string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    
    // if ( this->type == E_TP_FLOAT  ||  this->type == E_TP_DOUBLE ) {
    //     MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT);
    //     throw e;
    // }


    DataValue retVal;
    retVal.type = this->type;

    switch( this->type )
    {
    case E_TP_U_INT:
        retVal.value.uint_val = this->value.uint_val >> right.value.uint_val;
        break;
    case E_TP_S_INT:
        retVal.value.sint_val = this->value.sint_val >> right.value.sint_val;
        break;
    case E_TP_U_LONG:
        retVal.value.ulong_val = this->value.ulong_val >> right.value.ulong_val;
        break;
    case E_TP_S_LONG:
        retVal.value.slong_val = this->value.slong_val >> right.value.slong_val;
        break;
    case E_TP_U_LONG_LONG:
        retVal.value.ulonglong_val = this->value.ulonglong_val >> right.value.ulonglong_val;
        break;
    case E_TP_S_LONG_LONG:
        retVal.value.slonglong_val = this->value.slonglong_val >> right.value.slonglong_val;
        break;
    //case E_TP_FLOAT:
    //    retVal.value.float_val = this->value.float_val % right.value.float_val;
    //    break;
    //case E_TP_DOUBLE:
    //    retVal.value.double_val = this->value.double_val % right.value.double_val;
    //    break;
    default:
        {
            MyException e(E_THROW_CODE_CANNOT_REACH_HERE);

            std::string detail( __FILE__ );
            detail += (string(" @line : ") + to_string(__LINE__));

            e.setDetail(detail);
            throw e;
        }
        break;
    }

    return retVal;
}


void DataValue::doAssignment(const DataValue& rightVal)
{
    switch( this->type )
    {
    case E_TP_CHAR:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.char_val = static_cast<char>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.char_val = static_cast<char>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.char_val = static_cast<char>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.char_val = static_cast<char>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.char_val = static_cast<char>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.char_val = static_cast<char>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.char_val = static_cast<char>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.char_val = static_cast<char>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.char_val = static_cast<char>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.char_val = static_cast<char>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.char_val = static_cast<char>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.char_val = static_cast<char>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.char_val = static_cast<char>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_U_CHAR:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.uchar_val = static_cast<unsigned char>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.uchar_val = static_cast<unsigned char>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_S_CHAR:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.schar_val = static_cast<signed char>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.schar_val = static_cast<signed char>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.schar_val = static_cast<signed char>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.schar_val = static_cast<signed char>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.schar_val = static_cast<signed char>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.schar_val = static_cast<signed char>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.schar_val = static_cast<signed char>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.schar_val = static_cast<signed char>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.schar_val = static_cast<signed char>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.schar_val = static_cast<signed char>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.schar_val = static_cast<signed char>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.schar_val = static_cast<signed char>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.schar_val = static_cast<signed char>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }

        break;

    case E_TP_U_SHORT:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.ushort_val = static_cast<unsigned short>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.ushort_val = static_cast<unsigned short>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_S_SHORT:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.sshort_val = static_cast<signed short>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.sshort_val = static_cast<signed short>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;

    case E_TP_U_INT:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.uint_val = static_cast<unsigned int>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.uint_val = static_cast<unsigned int>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_S_INT:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.sint_val = static_cast<signed int>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.sint_val = static_cast<signed int>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.sint_val = static_cast<signed int>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.sint_val = static_cast<signed int>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.sint_val = static_cast<signed int>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.sint_val = static_cast<signed int>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.sint_val = static_cast<signed int>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.sint_val = static_cast<signed int>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.sint_val = static_cast<signed int>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.sint_val = static_cast<signed int>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.sint_val = static_cast<signed int>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.sint_val = static_cast<signed int>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.sint_val = static_cast<signed int>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;

    case E_TP_U_LONG:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.ulong_val = static_cast<unsigned long>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.ulong_val = static_cast<unsigned long>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_S_LONG:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.slong_val = static_cast<signed long>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.slong_val = static_cast<signed long>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.slong_val = static_cast<signed long>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.slong_val = static_cast<signed long>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.slong_val = static_cast<signed long>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.slong_val = static_cast<signed long>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.slong_val = static_cast<signed long>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.slong_val = static_cast<signed long>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.slong_val = static_cast<signed long>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.slong_val = static_cast<signed long>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.slong_val = static_cast<signed long>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.slong_val = static_cast<signed long>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.slong_val = static_cast<signed long>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;

    case E_TP_U_LONG_LONG:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.ulonglong_val = static_cast<unsigned long long>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.ulonglong_val = static_cast<unsigned long long>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_S_LONG_LONG:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.slonglong_val = static_cast<signed long long>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.slonglong_val = static_cast<signed long long>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;

    case E_TP_FLOAT:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.float_val = static_cast<float>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.float_val = static_cast<float>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.float_val = static_cast<float>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.float_val = static_cast<float>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.float_val = static_cast<float>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.float_val = static_cast<float>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.float_val = static_cast<float>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.float_val = static_cast<float>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.float_val = static_cast<float>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.float_val = static_cast<float>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.float_val = static_cast<float>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.float_val = static_cast<float>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.float_val = static_cast<float>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    case E_TP_DOUBLE:
        {
            switch( rightVal.type )
            {
            case E_TP_CHAR:
                this->value.double_val = static_cast<double>(rightVal.value.char_val);
                break;
            case E_TP_U_CHAR:
                this->value.double_val = static_cast<double>( rightVal.value.uchar_val );
                break;
            case E_TP_S_CHAR:
                this->value.double_val = static_cast<double>( rightVal.value.schar_val );
                break;

            case E_TP_U_SHORT:
                this->value.double_val = static_cast<double>( rightVal.value.ushort_val );
                break;
            case E_TP_S_SHORT:
                this->value.double_val = static_cast<double>( rightVal.value.sshort_val );
                break;

            case E_TP_U_INT:
                this->value.double_val = static_cast<double>( rightVal.value.uint_val );
                break;
            case E_TP_S_INT:
                this->value.double_val = static_cast<double>( rightVal.value.sint_val );
                break;

            case E_TP_U_LONG:
                this->value.double_val = static_cast<double>( rightVal.value.ulong_val );
                break;
            case E_TP_S_LONG:
                this->value.double_val = static_cast<double>( rightVal.value.slong_val );
                break;

            case E_TP_U_LONG_LONG:
                this->value.double_val = static_cast<double>( rightVal.value.ulonglong_val );
                break;
            case E_TP_S_LONG_LONG:
                this->value.double_val = static_cast<double>( rightVal.value.slonglong_val );
                break;

            case E_TP_FLOAT:
                this->value.double_val = static_cast<double>( rightVal.value.float_val );
                break;
            case E_TP_DOUBLE:
                this->value.double_val = static_cast<double>( rightVal.value.double_val );
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }


    this->type = rightVal.type;

}



