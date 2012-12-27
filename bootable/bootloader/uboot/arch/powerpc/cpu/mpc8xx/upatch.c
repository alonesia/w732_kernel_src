/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#include <common.h>
#include <commproc.h>

#if defined(CONFIG_SYS_I2C_UCODE_PATCH) || defined(CONFIG_SYS_SPI_UCODE_PATCH) || \
    defined(CONFIG_SYS_SMC_UCODE_PATCH)

static void UcodeCopy (volatile cpm8xx_t *cpm);

void cpm_load_patch (volatile immap_t *immr)
{
	immr->im_cpm.cp_rccr &= ~0x0003;	/* Disable microcode program area */

	UcodeCopy ((cpm8xx_t *)&immr->im_cpm);	/* Copy ucode patch to DPRAM   */
#ifdef CONFIG_SYS_SPI_UCODE_PATCH
    {
	volatile spi_t *spi = (spi_t *) & immr->im_cpm.cp_dparam[PROFF_SPI];
	/* Activate the microcode per the instructions in the microcode manual */
	/* NOTE:  We're only relocating the SPI parameters (not I2C).          */
	immr->im_cpm.cp_cpmcr1 = 0x802a;	/* Write Trap register 1 value */
	immr->im_cpm.cp_cpmcr2 = 0x8028;	/* Write Trap register 2 value */
	spi->spi_rpbase = CONFIG_SYS_SPI_DPMEM_OFFSET;	/* Where to relocte SPI params */
    }
#endif

#ifdef CONFIG_SYS_I2C_UCODE_PATCH
    {
	volatile iic_t *iip = (iic_t *) & immr->im_cpm.cp_dparam[PROFF_IIC];
	/* Activate the microcode per the instructions in the microcode manual */
	/* NOTE:  We're only relocating the I2C parameters (not SPI).          */
	immr->im_cpm.cp_cpmcr3 = 0x802e;	/* Write Trap register 3 value */
	immr->im_cpm.cp_cpmcr4 = 0x802c;	/* Write Trap register 4 value */
	iip->iic_rpbase = CONFIG_SYS_I2C_DPMEM_OFFSET;	/* Where to relocte I2C params */
    }
#endif

#ifdef CONFIG_SYS_SMC_UCODE_PATCH
    {
	volatile smc_uart_t *up = (smc_uart_t *) & immr->im_cpm.cp_dparam[PROFF_SMC1];
	/* Activate the microcode per the instructions in the microcode manual */
	/* NOTE:  We're only relocating the SMC parameters.                    */
	immr->im_cpm.cp_cpmcr1 = 0x8080;	/* Write Trap register 1 value */
	immr->im_cpm.cp_cpmcr2 = 0x8088;	/* Write Trap register 2 value */
	up->smc_rpbase = CONFIG_SYS_SMC_DPMEM_OFFSET;	/* Where to relocte SMC params */
    }
#endif

	/*
	 * Enable DPRAM microcode to execute from the first 512 bytes
	 * and a 256 byte extension of DPRAM.
	 */
#ifdef CONFIG_SYS_SMC_UCODE_PATCH
	immr->im_cpm.cp_rccr |= 0x0002;
#else
	immr->im_cpm.cp_rccr |= 0x0001;
#endif
}

#if defined(CONFIG_SYS_I2C_UCODE_PATCH) || defined(CONFIG_SYS_SPI_UCODE_PATCh)
static ulong patch_2000[] = {
	0x7FFFEFD9, 0x3FFD0000, 0x7FFB49F7, 0x7FF90000,
	0x5FEFADF7, 0x5F88ADF7, 0x5FEFAFF7, 0x5F88AFF7,
	0x3A9CFBC8, 0x77CAE1BB, 0xF4DE7FAD, 0xABAE9330,
	0x4E08FDCF, 0x6E0FAFF8, 0x7CCF76CF, 0xFDAFF9CF,
	0xABF88DC8, 0xAB5879F7, 0xB0927383, 0xDFD079F7,
	0xB090E6BB, 0xE5BBE74F, 0xB3FA6F0F, 0x6FFB76CE,
	0xEE0CF9CF, 0x2BFBEFEF, 0xCFEEF9CF, 0x76CEAD23,
	0x90B3DF99, 0x7FDDD0C1, 0x4BF847FD, 0x7CCF76CE,
	0xCFEF77CA, 0x7EAF7FAD, 0x7DFDF0B7, 0xEF7A7FCA,
	0x77CAFBC8, 0x6079E722, 0xFBC85FFF, 0xDFFF5FB3,
	0xFFFBFBC8, 0xF3C894A5, 0xE7C9EDF9, 0x7F9A7FAD,
	0x5F36AFE8, 0x5F5BFFDF, 0xDF95CB9E, 0xAF7D5FC3,
	0xAFED8C1B, 0x5FC3AFDD, 0x5FC5DF99, 0x7EFDB0B3,
	0x5FB3FFFE, 0xABAE5FB3, 0xFFFE5FD0, 0x600BE6BB,
	0x600B5FD0, 0xDFC827FB, 0xEFDF5FCA, 0xCFDE3A9C,
	0xE7C9EDF9, 0xF3C87F9E, 0x54CA7FED, 0x2D3A3637,
	0x756F7E9A, 0xF1CE37EF, 0x2E677FEE, 0x10EBADF8,
	0xEFDECFEA, 0xE52F7D9F, 0xE12BF1CE, 0x5F647E9A,
	0x4DF8CFEA, 0x5F717D9B, 0xEFEECFEA, 0x5F73E522,
	0xEFDE5F73, 0xCFDA0B61, 0x7385DF61, 0xE7C9EDF9,
	0x7E9A30D5, 0x1458BFFF, 0xF3C85FFF, 0xDFFFA7F8,
	0x5F5BBFFE, 0x7F7D10D0, 0x144D5F33, 0xBFFFAF78,
	0x5F5BBFFD, 0xA7F85F33, 0xBFFE77FD, 0x30BD4E08,
	0xFDCFE5FF, 0x6E0FAFF8, 0x7EEF7E9F, 0xFDEFF1CF,
	0x5F17ABF8, 0x0D5B5F5B, 0xFFEF79F7, 0x309EAFDD,
	0x5F3147F8, 0x5F31AFED, 0x7FDD50AF, 0x497847FD,
	0x7F9E7FED, 0x7DFD70A9, 0xEF7E7ECE, 0x6BA07F9E,
	0x2D227EFD, 0x30DB5F5B, 0xFFFD5F5B, 0xFFEF5F5B,
	0xFFDF0C9C, 0xAFED0A9A, 0xAFDD0C37, 0x5F37AFBD,
	0x7FBDB081, 0x5F8147F8,
};

static ulong patch_2F00[] = {
	0x3E303430, 0x34343737, 0xABBF9B99, 0x4B4FBDBD,
	0x59949334, 0x9FFF37FB, 0x9B177DD9, 0x936956BB,
	0xFBDD697B, 0xDD2FD113, 0x1DB9F7BB, 0x36313963,
	0x79373369, 0x3193137F, 0x7331737A, 0xF7BB9B99,
	0x9BB19795, 0x77FDFD3D, 0x573B773F, 0x737933F7,
	0xB991D115, 0x31699315, 0x31531694, 0xBF4FBDBD,
	0x35931497, 0x35376956, 0xBD697B9D, 0x96931313,
	0x19797937, 0x69350000,
};
#else

static ulong patch_2000[] = {
	0x3fff0000, 0x3ffd0000, 0x3ffb0000, 0x3ff90000,
	0x5fefeff8, 0x5f91eff8, 0x3ff30000, 0x3ff10000,
	0x3a11e710, 0xedf0ccb9, 0xf318ed66, 0x7f0e5fe2,
	0x7fedbb38, 0x3afe7468, 0x7fedf4d8, 0x8ffbb92d,
	0xb83b77fd, 0xb0bb5eb9, 0xdfda7fed, 0x90bde74d,
	0x6f0dcbd3, 0xe7decfed, 0xcb50cfed, 0xcfeddf6d,
	0x914d4f74, 0x5eaedfcb, 0x9ee0e7df, 0xefbb6ffb,
	0xe7ef7f0e, 0x9ee57fed, 0xebb7effa, 0xeb30affb,
	0x7fea90b3, 0x7e0cf09f, 0xbffff318, 0x5fffdfff,
	0xac35efea, 0x7fce1fc1, 0xe2ff5fbd, 0xaffbe2ff,
	0x5fbfaffb, 0xf9a87d0f, 0xaef8770f, 0x7d0fb0a2,
	0xeffbbfff, 0xcfef5fba, 0x7d0fbfff, 0x5fba4cf8,
	0x7fddd09b, 0x49f847fd, 0x7efdf097, 0x7fedfffd,
	0x7dfdf093, 0xef7e7e1e, 0x5fba7f0e, 0x3a11e710,
	0xedf0cc87, 0xfb18ad0a, 0x1f85bbb8, 0x74283b7e,
	0x7375e4bb, 0x2ab64fb8, 0x5c7de4bb, 0x32fdffbf,
	0x5f0843f8, 0x7ce3e1bb, 0xe74f7ded, 0x6f0f4fe8,
	0xc7ba32be, 0x73f2efeb, 0x600b4f78, 0xe5bb760b,
	0x5388aef8, 0x4ef80b6a, 0xcfef9ee5, 0xabf8751f,
	0xefef5b88, 0x741f4fe8, 0x751e760d, 0x7fdb70dd,
	0x741cafce, 0xefcc7fce, 0x751e7088, 0x741ce7bb,
	0x334ecfed, 0xafdbefeb, 0xe5bb760b, 0x53ceaef8,
	0xafe8e7eb, 0x4bf8771e, 0x7e007fed, 0x4fcbe2cc,
	0x7fbc3085, 0x7b0f7a0f, 0x34b177fd, 0xb0e75e93,
	0xdf313e3b, 0xaf78741f, 0x741f30cc, 0xcfef5f08,
	0x741f3e88, 0xafb8771e, 0x5f437fed, 0x0bafe2cc,
	0x741ccfec, 0xe5ca53a9, 0x6fcb4f74, 0x5e89df27,
	0x2a923d14, 0x4b8fdf0c, 0x751f741c, 0x6c1eeffa,
	0xefea7fce, 0x6ffc309a, 0xefec3fca, 0x308fdf0a,
	0xadf85e7a, 0xaf7daefd, 0x5e7adf0a, 0x5e7aafdd,
	0x761f1088, 0x1e7c7efd, 0x3089fffe, 0x4908fb18,
	0x5fffdfff, 0xafbbf0f7, 0x4ef85f43, 0xadf81489,
	0x7a0f7089, 0xcfef5089, 0x7a0fdf0c, 0x5e7cafed,
	0xbc6e780f, 0xefef780f, 0xefef790f, 0xa7f85eeb,
	0xffef790f, 0xefef790f, 0x1489df0a, 0x5e7aadfd,
	0x5f09fffb, 0xe79aded9, 0xeff96079, 0x607ae79a,
	0xded8eff9, 0x60795edb, 0x607acfef, 0xefefefdf,
	0xefbfef7f, 0xeeffedff, 0xebffe7ff, 0xafefafdf,
	0xafbfaf7f, 0xaeffadff, 0xabffa7ff, 0x6fef6fdf,
	0x6fbf6f7f, 0x6eff6dff, 0x6bff67ff, 0x2fef2fdf,
	0x2fbf2f7f, 0x2eff2dff, 0x2bff27ff, 0x4e08fd1f,
	0xe5ff6e0f, 0xaff87eef, 0x7e0ffdef, 0xf11f6079,
	0xabf8f51e, 0x7e0af11c, 0x37cfae16, 0x7fec909a,
	0xadf8efdc, 0xcfeae52f, 0x7d0fe12b, 0xf11c6079,
	0x7e0a4df8, 0xcfea5ea0, 0x7d0befec, 0xcfea5ea2,
	0xe522efdc, 0x5ea2cfda, 0x4e08fd1f, 0x6e0faff8,
	0x7c1f761f, 0xfdeff91f, 0x6079abf8, 0x761cee00,
	0xf91f2bfb, 0xefefcfec, 0xf91f6079, 0x761c27fb,
	0xefdf5e83, 0xcfdc7fdd, 0x50f84bf8, 0x47fd7c1f,
	0x761ccfcf, 0x7eef7fed, 0x7dfd70ef, 0xef7e7f1e,
	0x771efb18, 0x6079e722, 0xe6bbe5bb, 0x2e66e5bb,
	0x600b2ee1, 0xe2bbe2bb, 0xe2bbe2bb, 0x2f5ee2bb,
	0xe2bb2ff9, 0x6079e2bb,
};

static ulong patch_2F00[] = {
	0x30303030, 0x3e3e3030, 0xaf79b9b3, 0xbaa3b979,
	0x9693369f, 0x79f79777, 0x97333fff, 0xfb3b9e9f,
	0x79b91d11, 0x9e13f3ff, 0x3f9b6bd9, 0xe173d136,
	0x695669d1, 0x697b3daf, 0x79b93a3a, 0x3f979f91,
	0x379ff976, 0xf99777fd, 0x9779737d, 0xe9d6bbf9,
	0xbfffd9df, 0x97f7fd97, 0x6f7b9bff, 0xf9bd9683,
	0x397db973, 0xd97b3b9f, 0xd7f9f733, 0x9993bb9e,
	0xe1f9ef93, 0x73773337, 0xb936917d, 0x11f87379,
	0xb979d336, 0x8b7ded73, 0x1b7d9337, 0x31f3f22f,
	0x3f2327ee, 0xeeeeeeee, 0xeeeeeeee, 0xeeeeeeee,
	0xeeeeee4b, 0xf4fbdbd2, 0x58bb1878, 0x577fdfd2,
	0xd573b773, 0xf7374b4f, 0xbdbd25b8, 0xb177d2d1,
	0x7376856b, 0xbfdd687b, 0xdd2fff8f, 0x78ffff8f,
	0xf22f0000,
};
#endif

static void UcodeCopy (volatile cpm8xx_t *cpm)
{
	vu_long *p;
	int i;

	p = (vu_long *)&(cpm->cp_dpmem[0x0000]);
	for (i=0; i < sizeof(patch_2000)/4; ++i) {
		p[i] = patch_2000[i];
	}

	p = (vu_long *)&(cpm->cp_dpmem[0x0F00]);
	for (i=0; i < sizeof(patch_2F00)/4; ++i) {
		p[i] = patch_2F00[i];
	}
}

#endif	/* CONFIG_SYS_I2C_UCODE_PATCH, CONFIG_SYS_SPI_UCODE_PATCH */