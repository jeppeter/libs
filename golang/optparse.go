package optparse

type OptionParser struct{

}

type Option_add_param struct{
	flags []string
	help string
	defvalue string
	callback bool
	store bool
	store_const bool
	store_true bool
	append bool
	append_const bool
	count bool

}
func  (o *OptionParser) Add_option(opt option_add_param) e error{
	
}