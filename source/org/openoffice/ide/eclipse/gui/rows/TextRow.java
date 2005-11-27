/*************************************************************************
 *
 * $RCSfile: TextRow.java,v $
 *
 * $Revision: 1.2 $
 *
 * last change: $Author: cedricbosdo $ $Date: 2005/11/27 17:48:22 $
 *
 * The Contents of this file are made available subject to the terms of
 * either of the GNU Lesser General Public License Version 2.1
 *
 * Sun Microsystems Inc., October, 2000
 *
 *
 * GNU Lesser General Public License Version 2.1
 * =============================================
 * Copyright 2000 by Sun Microsystems, Inc.
 * 901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 * 
 * The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 * Copyright: 2002 by Sun Microsystems, Inc.
 *
 * All Rights Reserved.
 *
 * Contributor(s): Cedric Bosdonnat
 *
 *
 ************************************************************************/
package org.openoffice.ide.eclipse.gui.rows;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

/**
 * TODOC
 * 
 * @author cbosdonnat
 *
 */
public class TextRow extends LabeledRow implements FocusListener{
	
	private String value = new String();
	
	public TextRow(Composite parent, String property, String label){
		super(property);
		
		Label aLabel = new Label(parent, SWT.LEFT | SWT.SHADOW_NONE);
		aLabel.setText(label);
		Text aField = new Text(parent, SWT.BORDER);
		
		createContent(parent, aLabel, aField, null);
		field.addFocusListener(this);
	}

	public void focusGained(FocusEvent e) {
		// Ne fait rien...
	}

	/**
	 * Cette m�thode v�rifie si la valeur a chang�e et envoie un evenement
	 * au listener si tel est le cas.
	 * 
	 * @param e FocusEvent recu
	 */
	public void focusLost(FocusEvent e) {
		if (!((Text)field).getText().equals(value)){
			setValue(((Text)field).getText());
		}
	}

	public String getValue() {
		return value;
	}
	
	public void setValue(String aValue){
		String newText = aValue;
		if (null == aValue){
			newText = "";
		}
		
		if (!((Text)field).getText().equals(newText)){
			((Text)field).setText(newText);
		}
		
		value = newText;
		FieldEvent fe = new FieldEvent(getProperty(), getValue());
		fireFieldChangedEvent(fe);
		
		
	}
}
