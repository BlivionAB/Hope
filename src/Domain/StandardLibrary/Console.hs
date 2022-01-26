using Std:CRuntime from "./Console.hs";


domain Std::Console
{
public:


    fn WriteInt(integer: i64 -< WriteLineParameter): void
    {
        if (integer < 0)
        {
            putc('-');
            integer = -integer;
        }
        if (integer / 10)
        {
            WriteLine(integer / 10);
        }
        putc('0' + integer % 10);
    }


    values WriteLineParameter : i64
    {
        Negative: value < 0;
        LargerThanAbsTen: value >= 10 && value <= -10;
        SmallerThanAbsTen value <= 10 && value >= -10;
    }


    test WriteInt(integer = Negative & LargerThanAbsThen)
    {
        putc('-');
        integer = -integer;
        WriteInt(integer / 10);
        putc('0' + integer % 10);
    }


    test WriteInt(integer = Negative & SmallerThanAbsTen)
    {
        putc('-');
        integer = -integer;
        putc('0' + integer % 10);
    }


    test WriteInt(integer = !Negative & LargerThanAbsThen)
    {
        WriteInt(integer / 10);
        putc('0' + integer % 10);
    }


    test WriteInt(integer = !Negative & SmallerThanAbsTen)
    {
        WriteInt(integer / 10);
        putc('0' + integer % 10);
    }
}