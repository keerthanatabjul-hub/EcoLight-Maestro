void delay_us(unsigned int dlyus)
{
	for(dlyus*=12;dlyus;dlyus--);
}	
void delay_ms(unsigned int dlyms)
{
	for(dlyms*=12000;dlyms;dlyms--);
}	
void delay_s(unsigned int dlys)
{
	for(dlys*=12000000;dlys;dlys--);
}	

