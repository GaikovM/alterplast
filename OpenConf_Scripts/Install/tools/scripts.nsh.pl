#!perl -w

use strict;
use locale;
use File::Find;
use Getopt::Long;

use vars qw( 
	$opt_d $opt_u $opt_f $opt_i @ignores @includes @tmp @oldargs
	%used_includes $un $re_scripts_ext $first_subsect $cur_subsec
);

#==============================================================================

=head1 NAME

	scripts.nsh.pl - ��������� NSIS-�������� ���������/������������� �������� ��� OpenConf

=head1 SYNOPSIS

perl scripts.nsh.pl -d D:\home\KuntashovAM\projects\OpenConf_Scripts\������� > scripts.nsh

perl scripts.nsh.pl -d C:\1Cv77\bin\config\scripts -f filters.txt -i scripts > scripts.nsh

perl scripts.nsh.pl -u -d C:\1Cv77\bin\config\scripts -f filters.txt -i scripts >> scripts.nsh
	
=head1 DESCRIPTION

	������ ������������ ��� �������������� ��������� NSIS-��������� ��������� �������� 
��� OpenConf'� �� ������ ��������� ��������, � ������� ������������� ��� �������.

	� �������� ������ �������� (������� ��� ��������) ����� ����� ������������ ������� 
scripts ��������� ����������� OpenConf (�� ������ ������� ������������ �����������), 
���� ������� ������� ������� ����� ������ OpenConf_Scripts ����������� cvs.alterplast.ru.

	���� ������������ ��� ��������� ��������� � stdin. ��� ���������� ����������������
���� � ���� ������� ������������ ����������� ������� ��������������� ������ � ���� (> � >>).

	�������� ������������ ��������� �������: ���������� ��������� �������� �������,
� ��� ������� �������� �������� (����������� ��� �����) ���������� �������, ������� 
���������� ����� ��������, ��������������� ����� ��������.
	
	���� ��������� ������� (���)�������� - ����������, �� ������������ ��� ��������� �
����� �� ������, ��� � ��� ����� �����������. ���� ��������� ������� - ��� ����, �� 
������������ ��� ������ �� ��������� ��������� ����� �����. � ���������� �������� 
NSIS-�������� ����� ���������:

	;; ...
	SubSection "������������������"
		Section "cvs.vbs"
			SectionIn 1
			!insertmacro OC_STATUS "������� | cvs.vbs"
			SetOutPath "$INSTDIR\config\scripts\������������������"
			File "C:\1Cv77\bin\config\scripts\������������������\cvs.vbs"		
		SectionEnd ;; "cvs.vbs"
	
		;; ... �����-�� ������ ������ ...
		
		Section "��������������.vbs"
			SectionIn 1
			!insertmacro OC_STATUS "������� | ��������������.vbs"
			SetOutPath "$INSTDIR\config\scripts\������������������"
			File "C:\1Cv77\bin\config\scripts\������������������\��������������.vbs"
		SectionEnd ;; "��������������.vbs"	
	SubSectionEnd
	;; ...

	��� ������� ������� � ���������� -u ����� ������������ �������� ������������� �
����������� ���������� ������ (FIXME �� ����� ���� ����� �������� �� �����, � ��������,
���� ������ ����� ����������� � ������ ��������� ������ "�������" ������ ������������� 
��������).
	
	����� �������� ���������������� �� ���������� (��. ������������� ���������� 
$re_scripts_ext ���� �� ����), ��� ��������� ����� ������������ � � �������� ���������
�� ��������. ����� ������������ ��������� ���������� CVS.

	��� ����, ����� ��������� ��������� ������-���� ������� � �������� ��������� (� ������
� � ��� �����������), ������������� ��������� �����-�������� � ���� ���������� ���������, 
����������� � ����� ��������������� �����. ���� ���������� ��������� ������ ��� �����,
�� ����� ���� ������������ (�.�. �� �������� � ��������).

	������� ������ ���� �������� � ��������� ����, ������ ������ ������������� �� ����� 
������ ���������� �������:

	ignore:����������_���������

��������:

	ignore:common.vbs$
	ignore:������ ���� trad.vbs$
	
����� ignore �����������, ����� ��� �� ������ ���� ��������. ��� ������������� �����
��������, ���� � ���� ���� ������� � �������� �������� ����� -f.

	������ ������ ���������� ����������� �������� ��������� ��������, �.�. ��������, ��������
���������� �� ����� ����������� ������� � ������� �������� � ����������� ��������� ��������� 
��������.

	� ����������� ������� ����� ������ ��������, ��������� �������� ����� �������� ������������
����� ������ � �� ���������� ������� ������ �������������� ������.

	��� ��������� ������� ������������ ��������� ������. ��� "�������" � ��������� ��������
(��������, ����� ��� Intellisence.vbs, ������� ������� ������������� ��������� *.ints-������)
������� ��������� �������� � ������������� ���-������ � ����� �����. ��� ����� ��������
������ ��������� � ������ ����� ������� � ����� ���������� nsh.

	� ���� � �����-��������� ����������� ������������� ������� ������������ ����, ������
������ ����� ignore ������������ ����� include:

	include:����������_���������

��������:
	
	include:\WIntellisence.vbs$
	
	��� � � ������ � �����-��������� ������ ��� ����� �� ������� ���������� ����������� ��
����������� include-�������� � � ������ �������������� ������ (���� �� �������� ������
���) ������ �������� ��������� ������������ ��������� ��������� �������� ����� ��������
(!include), ��� ����������� �������� ������� ����� ��, ��� � ��� ��������������� ����� - ��� 
��������� ���������, ��� �� "un.���������������" - ��� ��������� ������������� (�.�. �����������
������� "un."):

	!include "Intellisence.nsh

��� 

	!include "un.Intellisence.nsh"

		����� ���� ������ ���� � �������� � ������-�������, ������� ����� �����������
� ����� ����� ��� ���������. ��� ����� ������������� ����� -i, � �������� �������� �������
������� ������� ���� � �������� � ����������� ����������.

	�����- � ������- ������� � ����� ����� ������������� � ������������ �������.
	
=cut

#==============================================================================

@oldargs = @ARGV;

$opt_u = $opt_d = $opt_f = $opt_i = "";

GetOptions('u'=>\$opt_u, 'd=s'=>\$opt_d, 'f=s'=>\$opt_f, 'i=s'=>\$opt_i);

$opt_d or die <<USAGE;
Usage: perl $0 [-u] -d path\\to\\dir [-f filters_list] [-i include_dir]
Parameters:
  -u              generate uninstall sections (add "un."-prefix to the section name)
  -f filter_list  use filters from specified filter_list file
  -i include_dir  include dir where *.nsh files are placed
USAGE

# ����������� �� ����� � ����� ����, ���� �� ����
$opt_d =~ s/[\/\\]$//;

-d $opt_d or die "Specified dir does not exists: $opt_d\n";
 
if ($opt_f) {

	-f $opt_f or die "Specified filter file (-f) not found: $opt_f";

	open FILTERS, "<$opt_f" or die("Can't open filter file $opt_f: $!");
	@tmp = (<FILTERS>);
	close FILTERS;
	
	@ignores	= grep s/^ignore:(.+?)\r*\n*$/$1/, @tmp;
	@includes	= grep s/^include:(.+?)\r*\n*$/$1/, @tmp;
	
	@tmp = ();
}

if ($opt_i) { 
	$opt_i =~ s/\//\\/;
	$opt_i .= "\\" unless $opt_i =~ /\\$/;
}

$un = $opt_u ? "un." : "";

$cur_subsec = "";
$re_scripts_ext = '\.(vbs|js|pl|pls|pys)$';

#==============================================================================

print <<HEADER;
;===========================================================================
;		������������� ������������� � ������� �������
; 			perl $0 @oldargs
; 		��� ��������� ��������� ��� ��������� ����� ��������!
; 		��������� ������� ������� � ���� $0.
;===========================================================================

HEADER

# �������... 
find (\&gen_nsh, $opt_d);

# ������� ��������� ���������

if ($un) { 
	# � ��������� ������������ ��������� ������ ��� 
	# �������� ��������������� ���������
	print <<SEC_RMDIR;
	Section "un.AfterUnInstall$cur_subsec"
		RMDir "\$INSTDIR\\config\\scripts\\$cur_subsec"
	SectionEnd
SEC_RMDIR
}

print qq(SubSectionEnd ;; $cur_subsec\n) if $cur_subsec;

#==============================================================================

sub gen_nsh {
	
	# ���������� CVS ��� �������� �� ����������
	!/(\.|CVS)$/ or return;
	$File::Find::dir !~ /CVS$/ or return;
	
	# ���� � $_ - ��� ����������, �� ��������� ����������
	# ��������� � ��������� ���������
	if (-d) {
		if ($cur_subsec) {
			if ($un) { 
				# � ��������� ������������ ��������� ������ ��� 
				# �������� ��������������� ���������
				print <<SEC_RMDIR;
	Section "un.AfterUnInstall$cur_subsec"
		RMDir "\$INSTDIR\\config\\scripts\\$cur_subsec"
	SectionEnd
SEC_RMDIR
			}
			print qq(SubSectionEnd ;; $un$cur_subsec);
		}
		print qq(\nSubSection $un"$_"\n);
		$cur_subsec = $_;
		return;
	}

	# ���� - ���� ������?
	-f && /$re_scripts_ext/ or return;	

	# ��� ����� ���������� ��������� ���� (����� �������� �����)
	$File::Find::name =~ s/\//\\/g; 
	
	# ���� ��� ����� �������� ���� �� ����� �����-��������, �� ���������� ���
	my @ret = grep { $File::Find::name =~ /$_/i } @ignores;
	return if scalar(@ret);

	# ��� ����� ���������� �������� ���������� �������� �����������?
	@ret = grep { $File::Find::name =~ /$_/i } @includes;
	if (scalar(@ret)) {
		# ��, ����� ������ ���������� ��� ��� ��� ���������
		my $fname = (/(.+)\.(\w+)$/)[0];
		print qq(\t!include "$opt_i$un$fname.nsh"\n);
		return;
	}
	
	# ���������� �������� �� �������������, ������� ���������� 
	# ����������� �������� ��������������
	if (!$opt_u) {
		print <<SECTION;
	Section "$_"
		SectionIn 1 2
		!insertmacro OC_STATUS "��������� �������� | $_ ..."
		SetOutPath "\$INSTDIR\\config\\scripts\\$cur_subsec"
		File "$File::Find::name"
	SectionEnd ;; $_
SECTION
;
	} 
	else {
		print <<SECTION;
	Section "un.$_"
		!insertmacro OC_STATUS "�������� �������� | $_ ..."
		Delete "\$INSTDIR\\config\\scripts\\$cur_subsec\\$_"
	SectionEnd ;; un.$_
SECTION
	} # if

} # &gen_nsh

#==============================================================================
